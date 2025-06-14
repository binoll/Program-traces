/// @file analysis_data.hpp
/// @brief Информация о запускавшемся ПО

#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

#include "../../../../parsers/prefetch/metadata/file_metric.hpp"
#include "../../../../parsers/prefetch/metadata/volume_info.hpp"

namespace WindowsDiskAnalysis {

/// @brief Информация о записи автозапуска
struct AutorunEntry {
  std::string name;      ///< Название записи автозапуска
  std::string path;      ///< Полный путь к исполняемому файлу
  std::string command;   ///< Командная строка запуска
  std::string location;  ///< Место расположения в реестре или файловой системе
};

/// @brief Информация о процессе
struct ProcessInfo {
  std::string filename;                ///< Имя файла
  std::vector<std::string> run_times;  ///< Временные метки запусков процесса
  uint32_t run_count = 0;              ///< Количество запусков процесса
  std::vector<PrefetchAnalysis::VolumeInfo> volumes;
  std::vector<PrefetchAnalysis::FileMetric> metrics;
};

/// @brief Информация о сетевом подключении
struct NetworkConnection {
  std::string process_name;    ///< Имя процесса, установившего соединение
  std::string local_address;   ///< Локальный IP-адрес
  std::string remote_address;  ///< Удалённый IP-адрес
  uint16_t port = 0;           ///< Номер порта
  std::string protocol;        ///< Протокол соединения (TCP/UDP)
};

}
