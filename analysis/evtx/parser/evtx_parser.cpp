#include "evtx_parser.hpp"

#include <regex>
#include <stdexcept>
#include <system_error>
#include <vector>

namespace EventLogAnalysis {

using namespace std::string_literals;

EvtxParser::EvtxParser() {
  libevtx_error_t* error = nullptr;
  if (libevtx_file_initialize(&evtx_file_, &error) != 1) {
    std::string error_msg = "Failed to initialize EVTX file handle";
    if (error) {
      char error_buffer[256];
      libevtx_error_sprint(error, error_buffer, sizeof(error_buffer));
      error_msg += ": "s + error_buffer;
      libevtx_error_free(&error);
    }
    throw std::runtime_error(error_msg);
  }
}

EvtxParser::~EvtxParser() {
  CloseLogFile();
  if (evtx_file_) {
    libevtx_file_free(&evtx_file_, nullptr);
  }
}

void EvtxParser::OpenLogFile(const std::string& file_path) {
  if (file_opened_) {
    CloseLogFile();
  }

  libevtx_error_t* error = nullptr;
  int access_flags = libevtx_get_access_flags_read();

  if (libevtx_file_open(evtx_file_, file_path.c_str(), access_flags, &error) !=
      1) {
    std::string error_msg = "Failed to open EVTX file: "s + file_path;
    if (error) {
      char error_buffer[256];
      libevtx_error_sprint(error, error_buffer, sizeof(error_buffer));
      error_msg += ": "s + error_buffer;
      libevtx_error_free(&error);
    }
    throw std::system_error(std::make_error_code(std::errc::io_error),
                            error_msg);
  }
  file_opened_ = true;
}

void EvtxParser::CloseLogFile() {
  if (file_opened_ && evtx_file_) {
    libevtx_file_close(evtx_file_, nullptr);
    file_opened_ = false;
  }
}

std::unique_ptr<EventData> EvtxParser::ParseRecord(libevtx_record_t* record) {
  auto event_data = std::make_unique<EventData>();
  libevtx_error_t* error = nullptr;

  // Get event identifier
  uint32_t event_id = 0;
  if (libevtx_record_get_event_identifier(record, &event_id, &error) == 1) {
    event_data->SetEventID(event_id);
  } else if (error) {
    libevtx_error_free(&error);
  }

  // Get written time (more reliable than creation time)
  uint64_t timestamp = 0;
  if (libevtx_record_get_written_time(record, &timestamp, &error) == 1) {
    event_data->SetTimestamp(timestamp);
  } else if (error) {
    libevtx_error_free(&error);
  }

  // Get event level
  uint8_t level = 0;
  if (libevtx_record_get_event_level(record, &level, &error) == 1) {
    event_data->SetLevel(static_cast<EventLevel>(level));
  } else if (error) {
    libevtx_error_free(&error);
  }

  // Get provider (UTF-8)
  size_t provider_size = 0;
  if (libevtx_record_get_utf8_provider_identifier_size(record, &provider_size,
                                                       &error) == 1) {
    if (provider_size > 0) {
      std::vector<char> buffer(provider_size);
      if (libevtx_record_get_utf8_provider_identifier(
              record, reinterpret_cast<uint8_t*>(buffer.data()), provider_size,
              &error) == 1) {
        event_data->SetProvider(buffer.data());
      } else if (error) {
        libevtx_error_free(&error);
      }
    }
  } else if (error) {
    libevtx_error_free(&error);
  }

  // Get computer name (UTF-8)
  size_t computer_size = 0;
  if (libevtx_record_get_utf8_computer_name_size(record, &computer_size,
                                                 &error) == 1) {
    if (computer_size > 0) {
      std::vector<char> buffer(computer_size);
      if (libevtx_record_get_utf8_computer_name(
              record, reinterpret_cast<uint8_t*>(buffer.data()), computer_size,
              &error) == 1) {
        event_data->SetComputer(buffer.data());
      } else if (error) {
        libevtx_error_free(&error);
      }
    }
  } else if (error) {
    libevtx_error_free(&error);
  }

  // Get channel name (UTF-8)
  size_t channel_size = 0;
  if (libevtx_record_get_utf8_channel_name_size(record, &channel_size,
                                                &error) == 1) {
    if (channel_size > 0) {
      std::vector<char> buffer(channel_size);
      if (libevtx_record_get_utf8_channel_name(
              record, reinterpret_cast<uint8_t*>(buffer.data()), channel_size,
              &error) == 1) {
        event_data->SetChannel(buffer.data());
      } else if (error) {
        libevtx_error_free(&error);
      }
    }
  } else if (error) {
    libevtx_error_free(&error);
  }

  // Get XML representation
  size_t xml_size = 0;
  if (libevtx_record_get_utf8_xml_string_size(record, &xml_size, &error) == 1) {
    if (xml_size > 0) {
      std::vector<char> buffer(xml_size);
      if (libevtx_record_get_utf8_xml_string(
              record, reinterpret_cast<uint8_t*>(buffer.data()), xml_size,
              &error) == 1) {
        std::string xml_string = buffer.data();
        event_data->SetXml(xml_string);
        // Extract event data from XML
        ExtractEventDataFromXml(*event_data, xml_string);
      } else if (error) {
        libevtx_error_free(&error);
      }
    }
  } else if (error) {
    libevtx_error_free(&error);
  }

  return event_data;
}

void EvtxParser::ExtractEventDataFromXml(EventData& event_data,
                                         const std::string& xml) {
  try {
    // Extract data from <EventData> elements
    std::regex data_regex(R"(<Data\s+Name="([^"]+)\"[^>]*>([^<]*)</Data>)");
    auto data_begin = std::sregex_iterator(xml.begin(), xml.end(), data_regex);
    auto data_end = std::sregex_iterator();

    for (auto i = data_begin; i != data_end; ++i) {
      const std::smatch& match = *i;
      if (match.size() == 3) {
        std::string name = match[1].str();
        std::string value = match[2].str();

        // Unescape XML entities
        size_t pos;
        while ((pos = value.find("&amp;")) != std::string::npos)
          value.replace(pos, 5, "&");
        while ((pos = value.find("&lt;")) != std::string::npos)
          value.replace(pos, 4, "<");
        while ((pos = value.find("&gt;")) != std::string::npos)
          value.replace(pos, 4, ">");
        while ((pos = value.find("&quot;")) != std::string::npos)
          value.replace(pos, 6, "\"");
        while ((pos = value.find("&apos;")) != std::string::npos)
          value.replace(pos, 6, "'");

        event_data.AddData(name, value);

        // Use CommandLine as description for process execution events
        if (name == "CommandLine") {
          event_data.SetDescription(value);
        }
      }
    }
  } catch (const std::regex_error& e) {
    // Handle regex errors
  }

  // Extract description if not set from CommandLine
  if (event_data.Description().empty()) {
    try {
      std::regex desc_regex(R"(<Description>([^<]+)</Description>)");
      std::smatch match;
      if (std::regex_search(xml, match, desc_regex) && match.size() > 1) {
        event_data.SetDescription(match[1].str());
      }
    } catch (const std::regex_error& e) {
      // Handle regex errors
    }
  }
}

std::vector<std::unique_ptr<IEventData>> EvtxParser::ParseEvents() {
  if (!file_opened_) {
    throw std::logic_error("No EVTX file opened");
  }

  libevtx_error_t* error = nullptr;
  int record_count = 0;
  std::vector<std::unique_ptr<IEventData>> events;

  // Get number of records
  if (libevtx_file_get_number_of_records(evtx_file_, &record_count, &error) !=
      1) {
    if (error) {
      char error_buffer[256];
      libevtx_error_sprint(error, error_buffer, sizeof(error_buffer));
      libevtx_error_free(&error);
      throw std::runtime_error("Failed to get record count: "s + error_buffer);
    }
    throw std::runtime_error("Failed to get record count");
  }

  // Retrieve all records
  for (int i = 0; i < record_count; i++) {
    libevtx_record_t* record = nullptr;
    if (libevtx_file_get_record_by_index(evtx_file_, i, &record, &error) == 1) {
      events.push_back(ParseRecord(record));
      libevtx_record_free(&record, nullptr);
    } else if (error) {
      libevtx_error_free(&error);
    }
  }

  return events;
}

std::vector<std::unique_ptr<IEventData>> EvtxParser::GetEventsByType(
    uint32_t event_id) {
  if (!file_opened_) {
    throw std::logic_error("No EVTX file opened");
  }

  libevtx_error_t* error = nullptr;
  int record_count = 0;
  std::vector<std::unique_ptr<IEventData>> filtered_events;

  // Get number of records
  if (libevtx_file_get_number_of_records(evtx_file_, &record_count, &error) !=
      1) {
    if (error) {
      char error_buffer[256];
      libevtx_error_sprint(error, error_buffer, sizeof(error_buffer));
      libevtx_error_free(&error);
      throw std::runtime_error("Failed to get record count: "s + error_buffer);
    }
    throw std::runtime_error("Failed to get record count");
  }

  // Filter records by event ID
  for (int i = 0; i < record_count; i++) {
    libevtx_record_t* record = nullptr;
    if (libevtx_file_get_record_by_index(evtx_file_, i, &record, &error) == 1) {
      uint32_t current_id = 0;
      if (libevtx_record_get_event_identifier(record, &current_id, nullptr) ==
              1 &&
          current_id == event_id) {
        filtered_events.push_back(ParseRecord(record));
      }
      libevtx_record_free(&record, nullptr);
    } else if (error) {
      libevtx_error_free(&error);
    }
  }

  return filtered_events;
}

}
