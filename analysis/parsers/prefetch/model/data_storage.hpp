/**
 * @file data_storage.hpp
 * @brief Структура хранения данных Prefetch-файла
 */

#pragma once

#include <ctime>
#include <vector>

#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"
#include "windows_version.hpp"

namespace PrefetchAnalysis {

/**
 * @struct PrefetchDataStorage
 * @brief Чистый контейнер для данных Prefetch-файла
 * @details Содержит все поля, необходимые для создания PrefetchData. Может
 * использоваться как строителем, так и другими фабриками
 */
struct PrefetchDataStorage {
  std::string executable_name;  ///< Исполняемое имя Prefetch-файла в формате
                                ///< "NAME.EXE-XXXXXXXX.pf"
  uint32_t prefetch_hash = 0;   ///< 32-битный хеш от пути исполняемого файла
  uint32_t run_count = 0;       ///< Количество запусков приложения
  std::vector<time_t>
      run_times;  ///< Список времён запусков в формате UNIX time (UTC)
  std::vector<VolumeInfo>
      volumes;  ///< Информация о логических дисках, использованных при запуске
  std::vector<FileMetric>
      metrics;  ///< Список файлов и соответствующих метрик использования
  uint8_t format_version =
      0;  ///< Версия формата Prefetch-файла (например, 17, 23, 26)
  uint64_t last_run_time =
      0;  ///< FILETIME последнего запуска в 64-битном формате
  std::string windows_version;  ///< Строковое представление версии Windows
};

}
