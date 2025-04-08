/**
 * @file prefetch_info.hpp
 * @brief Заголовочный файл с классами для хранения информации из Prefetch-файлов
 */

#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

namespace PrefetchAnalysis {

/**
 * @brief Класс для хранения информации о томе
 */
class VolumeInfo {
 public:
  /**
     * @brief Конструктор с инициализацией данных
     */
  VolumeInfo(std::string device_path, uint32_t serial, uint64_t create_time)
      : device_path_(std::move(device_path)),
        serial_number_(serial),
        creation_time_(create_time) {}

  // Геттеры
  std::string device_path() const { return device_path_; }
  uint32_t serial_number() const { return serial_number_; }
  uint64_t creation_time() const { return creation_time_; }

 private:
  std::string device_path_;  ///< Путь к устройству
  uint32_t serial_number_;   ///< Серийный номер тома
  uint64_t creation_time_;   ///< Время создания в FILETIME
};

/**
 * @brief Класс для хранения метрик файла
 */
class FileMetric {
 public:
  FileMetric(std::string name, uint64_t ref)
      : filename_(std::move(name)), file_reference_(ref) {}

  std::string filename() const { return filename_; }
  uint64_t file_reference() const { return file_reference_; }

 private:
  std::string filename_;     ///< Полное имя файла
  uint64_t file_reference_;  ///< Ссылка в MFT
};

/**
 * @brief Основной класс для хранения данных Prefetch
 */
class PrefetchData {
 public:
  // Геттеры
  std::string executable_name() const { return executable_name_; }
  uint32_t prefetch_hash() const { return prefetch_hash_; }
  uint32_t run_count() const { return run_count_; }
  const std::vector<time_t>& run_times() const { return run_times_; }
  const std::vector<VolumeInfo>& volumes() const { return volumes_; }
  const std::vector<FileMetric>& metrics() const { return metrics_; }

  // Сеттеры
  void set_executable_name(std::string name) {
    executable_name_ = std::move(name);
  }
  void set_prefetch_hash(uint32_t hash) { prefetch_hash_ = hash; }
  void set_run_count(uint32_t count) { run_count_ = count; }
  void add_run_time(time_t time) { run_times_.push_back(time); }
  void add_volume(VolumeInfo vol) { volumes_.push_back(vol); }
  void add_metric(FileMetric metric) { metrics_.push_back(metric); }

 private:
  std::string executable_name_;      ///< Имя исполняемого файла
  uint32_t prefetch_hash_;           ///< Хэш Prefetch
  uint32_t run_count_;               ///< Количество запусков
  std::vector<time_t> run_times_;    ///< Времена последних запусков
  std::vector<VolumeInfo> volumes_;  ///< Информация о томах
  std::vector<FileMetric> metrics_;  ///< Метрики файлов
};

}  // namespace prefetch
