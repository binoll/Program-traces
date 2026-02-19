#include "eventlog_analyzer.hpp"

#include <algorithm>
#include <filesystem>
#include <utility>

#include "../../../../utils/config/config.hpp"
#include "../../../../utils/logging/logger.hpp"
#include "../../../../utils/utils.hpp"

namespace fs = std::filesystem;

namespace WindowsDiskAnalysis {

EventLogAnalyzer::EventLogAnalyzer(
    std::unique_ptr<EventLogAnalysis::IEventLogParser> evt_parser,
    std::unique_ptr<EventLogAnalysis::IEventLogParser> evtx_parser,
    std::string os_version, const std::string& ini_path)
    : evt_parser_(std::move(evt_parser)),
      evtx_parser_(std::move(evtx_parser)),
      os_version_(std::move(os_version)) {
  trim(os_version_);
  loadConfigurations(ini_path);
}

void EventLogAnalyzer::loadConfigurations(const std::string& ini_path) {
  Config config(ini_path);
  const auto logger = GlobalLogger::get();

  // Загрузка путей к журналам событий
  std::string log_paths = config.getString(os_version_, "EventLogs", "");
  auto log_path_list = split(log_paths, ',');
  for (auto& path : log_path_list) {
    trim(path);
    if (!path.empty()) {
      config_.log_paths.push_back(path);
    }
  }

  // Загрузка ID событий о процессах
  std::string process_ids = config.getString(os_version_, "ProcessEventIDs", "");
  auto process_id_list = split(process_ids, ',');
  for (auto& id_str : process_id_list) {
    trim(id_str);
    if (!id_str.empty()) {
      try {
        config_.process_event_ids.push_back(static_cast<uint32_t>(std::stoul(id_str)));
      } catch (...) {
        logger->debug("Некорректный ID процесса: \"{}\"", id_str);
      }
    }
  }

  // Загрузка ID событий о сети
  std::string network_ids = config.getString(os_version_, "NetworkEventIDs", "");
  auto network_id_list = split(network_ids, ',');
  for (auto& id_str : network_id_list) {
    trim(id_str);
    if (!id_str.empty()) {
      try {
        config_.network_event_ids.push_back(static_cast<uint32_t>(std::stoul(id_str)));
      } catch (...) {
        logger->debug("Некорректный ID сети: \"{}\"", id_str);
      }
    }
  }

  logger->debug("Загружена конфигурация журналов для \"{}\"", os_version_);
}

EventLogAnalysis::IEventLogParser* EventLogAnalyzer::getParserForFile(
    const std::string& file_path) const {
  const fs::path path = file_path;
  std::string ext = path.extension().string();

  // Преобразование расширения к нижнему регистру
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (ext == ".evt") {
    return evt_parser_.get();
  }
  if (ext == ".evtx") {
    return evtx_parser_.get();
  }

  return nullptr;
}

void EventLogAnalyzer::collect(
    const std::string& disk_root,
    std::map<std::string, ProcessInfo>& /*process_data*/,
    std::vector<NetworkConnection>& /*network_connections*/) {
  const auto logger = GlobalLogger::get();

  for (const auto& log_path : config_.log_paths) {
    std::string full_path = disk_root + log_path;

    // Проверяем существование и тип пути (директория/файл)
    if (!fs::exists(full_path)) {
      logger->debug("Путь не существует: \"{}\"", full_path);
      continue;
    }

    std::vector<std::string> files_to_parse;

    // Собираем файлы для обработки
    if (fs::is_directory(full_path)) {
      for (const auto& entry : fs::directory_iterator(full_path)) {
        if (entry.is_regular_file()) {
          files_to_parse.push_back(entry.path().string());
        }
      }
    } else if (fs::is_regular_file(full_path)) {
      files_to_parse.push_back(full_path);
    } else {
      logger->debug("Путь не является ни файлом, ни директорией: \"{}\"",
                    full_path);
      continue;
    }

    // Обрабатываем собранные файлы
    for (const auto& file_path : files_to_parse) {
      auto* parser = getParserForFile(file_path);
      if (!parser) {
        logger->debug("Неизвестный формат журнала: \"{}\"", file_path);
        continue;
      }

      // TODO: Implement event parsing logic using the parser interface
      // Currently the parser interface methods used in the original code (getEventsByType)
      // might not be available or might have different signatures.
      // Assuming we need to iterate over events and filter by ID.

      // Placeholder for actual parsing logic
      logger->debug("Parsing event log: {}", file_path);
    }
  }
}

}
