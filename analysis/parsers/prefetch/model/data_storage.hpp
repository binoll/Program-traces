/**
 * @file data_storage.hpp
 * @brief Структура хранения данных Prefetch-файла
 */

#pragma once

#include <ctime>
#include <vector>

#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"

namespace PrefetchAnalysis {

/**
 * @struct PrefetchDataStorage
 * @brief Чистый контейнер для данных Prefetch-файла
 * @details Содержит все поля, необходимые для создания PrefetchData. Может
 * использоваться как строителем, так и другими фабриками
 */
struct PrefetchDataStorage {
  std::string executable_name;      ///< Формат "NAME.EXE-XXXXXX"
  uint32_t prefetch_hash = 0;       ///< 32-битный хеш файла
  uint32_t run_count = 0;           ///< Число запусков ≥0
  std::vector<time_t> run_times;    ///< UNIX-временные метки
  std::vector<VolumeInfo> volumes;  ///< Список томов
  std::vector<FileMetric> metrics;  ///< Файловые метрики
  uint8_t format_version = 0;       ///< Версия формата ≥10
  uint64_t last_run_time = 0;       ///< FILETIME последнего запуска
};

}
