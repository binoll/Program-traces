#include "parser.hpp"

#include <sstream>
#include <vector>

#include "../common/time_converter.hpp"
#include "../../model/event_data_builder.hpp"

namespace EventLogAnalysis {

EvtParser::EvtParser() {
  libevt_error_t* error = nullptr;
  if (libevt_file_initialize(&evt_file_, &error) != 1) {
    handleLibEvtError("Не удалось инициализировать libevt", error);
    throw std::runtime_error("Не удалось инициализировать парсер EVT-файлов");
  }
}

EvtParser::~EvtParser() {
  closeFile();
  if (evt_file_) {
    libevt_file_free(&evt_file_, nullptr);
  }
}

std::vector<EventData> EvtParser::parse(const std::string& file_path) {
  openFile(file_path);

  try {
    libevt_error_t* error = nullptr;
    int record_count = 0;

    if (libevt_file_get_number_of_records(evt_file_, &record_count, &error) !=
        1) {
      handleLibEvtError("Не удалось получить количество записей", error);
      throw std::runtime_error("Не удалось прочитать EVT файл");
    }

    std::vector<EventData> events;
    events.reserve(record_count);

    for (int i = 0; i < record_count; ++i) {
      libevt_record_t* record = nullptr;
      if (libevt_file_get_record_by_index(evt_file_, i, &record, &error) == 1) {
        events.push_back(parseRecord(record));
        libevt_record_free(&record, nullptr);
      } else {
        // Log error but continue processing other records
        if (error) {
            char buffer[256];
            libevt_error_sprint(error, buffer, sizeof(buffer));
            libevt_error_free(&error);
            // TODO: Log warning
        }
      }
    }

    return events;
  } catch (...) {
    closeFile();
    throw;
  }
}

std::vector<EventData> EvtParser::filterByEventId(const std::string& file_path,
                                                  uint32_t event_id) {
  openFile(file_path);

  try {
    libevt_error_t* error = nullptr;
    int record_count = 0;

    if (libevt_file_get_number_of_records(evt_file_, &record_count, &error) !=
        1) {
      handleLibEvtError("Не удалось получить количество записей", error);
      throw std::runtime_error("Не удалось прочитать EVT файл");
    }

    std::vector<EventData> events;

    for (int i = 0; i < record_count; ++i) {
      libevt_record_t* record = nullptr;
      if (libevt_file_get_record_by_index(evt_file_, i, &record, &error) == 1) {
        uint32_t current_id = 0;
        if (libevt_record_get_event_identifier(record, &current_id, nullptr) ==
                1 &&
            current_id == event_id) {
          events.push_back(parseRecord(record));
        }
        libevt_record_free(&record, nullptr);
      } else {
         if (error) {
            libevt_error_free(&error);
        }
      }
    }

    return events;
  } catch (...) {
    closeFile();
    throw;
  }
}

bool EvtParser::supports(const std::string& file_path) const {
  return file_path.length() > 4 &&
         file_path.substr(file_path.length() - 4) == ".evt";
}

std::string EvtParser::formatName() const {
  return "Журнал событий Windows (EVT)";
}

std::vector<std::string> EvtParser::supportedExtensions() const {
  return {".evt"};
}

void EvtParser::openFile(const std::string& file_path) {
  if (file_opened_) {
    closeFile();
  }

  libevt_error_t* error = nullptr;
  if (libevt_file_open(evt_file_, file_path.c_str(),
                       LIBEVT_ACCESS_FLAG_READ, &error) != 1) {
    handleLibEvtError("Не удалось открыть файл: " + file_path, error);
    throw std::runtime_error("Не удалось открыть файл EVT");
  }
  file_opened_ = true;
}

void EvtParser::closeFile() noexcept {
  if (file_opened_ && evt_file_) {
    libevt_file_close(evt_file_, nullptr);
    file_opened_ = false;
  }
}

EventData EvtParser::parseRecord(libevt_record_t* record) {
  EventDataBuilder builder;
  libevt_error_t* error = nullptr;

  // Основные поля события
  uint32_t event_id = 0;
  if (libevt_record_get_event_identifier(record, &event_id, &error) == 1) {
    builder.setEventId(event_id);
  } else if (error) {
      libevt_error_free(&error);
  }

  uint32_t written_time = 0;
  if (libevt_record_get_written_time(record, &written_time, &error) == 1) {
    builder.setTimestamp(TimeConverter::secondsSince1970ToFiletime(written_time));
  } else if (error) {
      libevt_error_free(&error);
  }

  uint16_t event_type = 0;
  if (libevt_record_get_event_type(record, &event_type, &error) == 1) {
    builder.setLevel(convertEventType(event_type));
  } else if (error) {
      libevt_error_free(&error);
  }

  // Строковые поля
  size_t size = 0;
  if (libevt_record_get_utf8_source_name_size(record, &size, &error) == 1 && size > 0) {
      std::vector<char> buffer(size);
      if (libevt_record_get_utf8_source_name(record, reinterpret_cast<uint8_t*>(buffer.data()), size, &error) == 1) {
          builder.setProvider(std::string(buffer.data()));
      }
      if (error) libevt_error_free(&error);
  } else if (error) {
      libevt_error_free(&error);
  }

  if (libevt_record_get_utf8_computer_name_size(record, &size, &error) == 1 && size > 0) {
      std::vector<char> buffer(size);
      if (libevt_record_get_utf8_computer_name(record, reinterpret_cast<uint8_t*>(buffer.data()), size, &error) == 1) {
          builder.setComputer(std::string(buffer.data()));
      }
      if (error) libevt_error_free(&error);
  } else if (error) {
      libevt_error_free(&error);
  }

  // Строковые данные события
  int string_count = 0;
  if (libevt_record_get_number_of_strings(record, &string_count, &error) == 1) {
    std::ostringstream description;

    for (int i = 0; i < string_count; ++i) {
      if (libevt_record_get_utf8_string_size(record, i, &size, &error) == 1 &&
          size > 0) {
        std::vector<char> buffer(size);
        if (libevt_record_get_utf8_string(
                record, i, reinterpret_cast<uint8_t*>(buffer.data()), size,
                &error) == 1) {
          std::string value(buffer.data());
          builder.addData("String" + std::to_string(i), value);

          if (i > 0) description << " | ";
          description << value;
        }
        if (error) libevt_error_free(&error);
      } else if (error) {
          libevt_error_free(&error);
      }
    }

    std::string desc = description.str();
    if (!desc.empty()) {
      builder.setDescription(desc);
    }
  } else if (error) {
      libevt_error_free(&error);
  }

  return std::move(builder).build();
}

EventLevel EvtParser::convertEventType(uint16_t event_type) noexcept {
  switch (event_type) {
    case LIBEVT_EVENT_TYPE_ERROR:
      return EventLevel::Error;
    case LIBEVT_EVENT_TYPE_WARNING:
      return EventLevel::Warning;
    case LIBEVT_EVENT_TYPE_INFORMATION:
    case LIBEVT_EVENT_TYPE_AUDIT_SUCCESS:
    case LIBEVT_EVENT_TYPE_AUDIT_FAILURE:
      return EventLevel::Info;
    default:
      return EventLevel::LogAlways;
  }
}

void EvtParser::handleLibEvtError(const std::string& context,
                                  libevt_error_t* error) {
  if (error) {
    char buffer[256];
    libevt_error_sprint(error, buffer, sizeof(buffer));
    libevt_error_free(&error);
    throw std::runtime_error(context + ": " + buffer);
  }
}

}
