/**
* @file PrefetchInfo.h
 * @brief Заголовочный файл, содержащий структуры данных для информации Prefetch
 */

#pragma once

#include <ctime>
#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Информация о томе
 */
struct VolumeInfo {
  uint64_t creationTime;    ///< Время создания тома в FILETIME
  uint32_t serialNumber;    ///< Серийный номер тома
  std::string devicePath;   ///< Путь к устройству
};

/**
 * @brief Метрика файла
 */
struct FileMetric {
  std::string filename;     ///< Имя файла
  uint64_t fileReference;   ///< Ссылка на файл
};

/**
 * @brief Основная информация о Prefetch файле
 */
struct PrefetchInfo {
  std::string executableName;          ///< Имя исполняемого файла
  uint32_t prefetchHash;               ///< Хэш Prefetch
  uint32_t runCount;                   ///< Количество запусков
  std::vector<time_t> lastRunTimes;    ///< Времена последних запусков
  std::vector<VolumeInfo> volumes;     ///< Информация о томах
  std::vector<FileMetric> fileMetrics; ///< Метрики файлов
};
