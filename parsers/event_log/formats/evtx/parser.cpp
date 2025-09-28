#include "parser.hpp"

#include "../common/time_converter.hpp"
#include "../common/xml_parser.hpp"
#include "parsers/event_log/model/event_data_builder.hpp"

namespace EventLogAnalysis {

EvtxParser::EvtxParser() {
  libevtx_error_t* error = nullptr;
  if (libevtx_file_initialize(&evtx_file_, &error) != 1) {
    handleLibEvtxError("Не удалось инициализировать libevtx", error);
    throw std::runtime_error("Не удалось инициализировать парсер EVTX файлов");
  }
}

EvtxParser::~EvtxParser() {
  closeFile();
  if (evtx_file_) {
    libevtx_file_free(&evtx_file_, nullptr);
  }
}

std::vector<EventData> EvtxParser::parse(const std::string& file_path) {
  openFile(file_path);

  try {
    libevtx_error_t* error = nullptr;
    int32_t record_count = 0;

    if (libevtx_file_get_number_of_records(evtx_file_, &record_count, &error) !=
        1) {
      handleLibEvtxError("Не удалось получить количество записей", error);
      throw std::runtime_error("Не удалось прочитать EVTX файл");
    }

    std::vector<EventData> events;
    events.reserve(record_count);

    for (int32_t i = 0; i < record_count; ++i) {
      libevtx_record_t* record = nullptr;
      if (libevtx_file_get_record_by_index(evtx_file_, i, &record, &error) ==
          1) {
        events.push_back(parseRecord(record));
        libevtx_record_free(&record, nullptr);
      } else {
        handleLibEvtxError("Не удалось прочитать запись " + std::to_string(i),
                           error);
      }
    }

    return events;
  } catch (...) {
    closeFile();
    throw;
  }
}

std::vector<EventData> EvtxParser::filterByEventId(const std::string& file_path,
                                                   uint32_t event_id) {
  openFile(file_path);

  try {
    libevtx_error_t* error = nullptr;
    int32_t record_count = 0;

    if (libevtx_file_get_number_of_records(evtx_file_, &record_count, &error) !=
        1) {
      handleLibEvtxError("Не удалось получить количество записей", error);
      throw std::runtime_error("Не удалось прочитать EVTX файл");
    }

    std::vector<EventData> events;

    for (int32_t i = 0; i < record_count; ++i) {
      libevtx_record_t* record = nullptr;
      if (libevtx_file_get_record_by_index(evtx_file_, i, &record, &error) ==
          1) {
        uint32_t current_id = 0;
        if (libevtx_record_get_event_identifier(record, &current_id, nullptr) ==
                1 &&
            current_id == event_id) {
          events.push_back(parseRecord(record));
        }
        libevtx_record_free(&record, nullptr);
      } else {
        handleLibEvtxError("Не удалось прочитать запись " + std::to_string(i),
                           error);
      }
    }

    return events;
  } catch (...) {
    closeFile();
    throw;
  }
}

bool EvtxParser::supports(const std::string& file_path) const {
  if (file_path.length() < 5) return false;

  const std::string ext = file_path.substr(file_path.length() - 5);
  return ext == ".evtx" || ext == ".evt-x";
}

std::string EvtxParser::formatName() const {
  return "Журнал событий Windows (EVTX)";
}

std::vector<std::string> EvtxParser::supportedExtensions() const {
  return {".evtx", ".evt-x"};
}

void EvtxParser::openFile(const std::string& file_path) {
  if (file_opened_) {
    closeFile();
  }

  libevtx_error_t* error = nullptr;
  if (libevtx_file_open(evtx_file_, file_path.c_str(),
                        libevtx_get_access_flags_read(), &error) != 1) {
    handleLibEvtxError("Не удалось открыть файл: " + file_path, error);
    throw std::runtime_error("Не удалось открыть файл EVTX");
  }
  file_opened_ = true;
}

void EvtxParser::closeFile() noexcept {
  if (file_opened_ && evtx_file_) {
    libevtx_file_close(evtx_file_, nullptr);
    file_opened_ = false;
  }
}

EventData EvtxParser::parseRecord(libevtx_record_t* record) {
  auto builder = EventData::builder();
  libevtx_error_t* error = nullptr;

  // Вспомогательная лямбда для безопасных вызовов libevtx
  auto safeGet = [&](auto getter, auto& result) -> bool {
    if (getter(record, &result, &error) == 1) return true;
    if (error) libevtx_error_free(&error);
    return false;
  };

  // Основные поля события
  uint32_t event_id = 0;
  if (safeGet(libevtx_record_get_event_identifier, event_id)) {
    builder.event_id(event_id);
  }

  uint64_t timestamp = 0;
  if (safeGet(libevtx_record_get_written_time, timestamp)) {
    builder.timestamp(timestamp);
  }

  uint8_t level = 0;
  if (safeGet(libevtx_record_get_event_level, level)) {
    builder.level(static_cast<EventLevel>(level));
  }

  // Строковые поля
  auto getStringField = [&](auto size_getter, auto value_getter, auto setter) {
    size_t size = 0;
    if (size_getter(record, &size, &error) == 1 && size > 0) {
      std::vector<char> buffer(size);
      if (value_getter(record, reinterpret_cast<uint8_t*>(buffer.data()), size,
                       &error) == 1) {
        setter(std::string(buffer.data()));
      }
      if (error) libevtx_error_free(&error);
    }
  };

  getStringField(libevtx_record_get_utf8_provider_identifier_size,
                 libevtx_record_get_utf8_provider_identifier,
                 [&](auto str) { builder.provider(std::move(str)); });

  getStringField(libevtx_record_get_utf8_computer_name_size,
                 libevtx_record_get_utf8_computer_name,
                 [&](auto str) { builder.computer(std::move(str)); });

  getStringField(libevtx_record_get_utf8_channel_name_size,
                 libevtx_record_get_utf8_channel_name,
                 [&](auto str) { builder.channel(std::move(str)); });

  // XML данные
  size_t xml_size = 0;
  if (libevtx_record_get_utf8_xml_string_size(record, &xml_size, &error) == 1 &&
      xml_size > 0) {
    std::vector<char> buffer(xml_size);
    if (libevtx_record_get_utf8_xml_string(
            record, reinterpret_cast<uint8_t*>(buffer.data()), xml_size,
            &error) == 1) {
      std::string xml(buffer.data());
      builder.xml(xml);

      // Извлекаем данные из XML
      const auto xml_data = XmlEventParser::parseEventData(xml);
      for (const auto& [key, value] : xml_data) {
        builder.add_data(key, value);
      }

      // Извлекаем описание
      const std::string description = XmlEventParser::parseDescription(xml);
      if (!description.empty()) {
        builder.description(description);
      }
    }
    if (error) libevtx_error_free(&error);
  }

  return std::move(builder).build();
}

void EvtxParser::handleLibEvtxError(const std::string& context,
                                    libevtx_error_t* error) {
  if (error) {
    char buffer[256];
    libevtx_error_sprint(error, buffer, sizeof(buffer));
    libevtx_error_free(&error);
    throw std::runtime_error(context + ": " + buffer);
  }
}

}
