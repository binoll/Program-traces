/// @file ievent_log_parser.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../data_model/event_data.hpp"

namespace EventLogAnalysis {

/// @brief Интерфейс для парсера журналов событий Windows
class IEventLogParser {
public:
  virtual ~IEventLogParser() = default;

  virtual void OpenLogFile(const std::string& file_path) = 0;
  virtual void CloseLogFile() = 0;
  virtual std::vector<std::unique_ptr<IEventData>> ParseEvents() = 0;
  virtual std::vector<std::unique_ptr<IEventData>> GetEventsByType(uint32_t event_id) = 0;
};

}
