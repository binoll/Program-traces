/**
 * @file prefetch_parser.cpp
 * @brief Реализация парсера Prefetch-файлов
 */

#include "prefetch_parser.hpp"
#include <sstream>

namespace PrefetchAnalysis {

using namespace std;

// Вспомогательная функция конвертации FILETIME
static time_t filetime_to_unix(uint64_t ft) {
  return static_cast<time_t>((ft - 116444736000000000ULL) / 10000000ULL);
}

void Parser::initialize() {
  if (libscca_file_initialize(&scca_file_, nullptr) != 1) {
    error_handler_->handle("Ошибка инициализации libscca", nullptr);
  }
}

PrefetchData Parser::parse(const string& path) {
  PrefetchData data;

  if (libscca_file_open(scca_file_, path.c_str(), LIBSCCA_ACCESS_FLAG_READ,
                        nullptr) != 1) {
    error_handler_->handle("Ошибка открытия файла", nullptr);
  }

  parse_basic_info(data);
  parse_run_history(data);
  parse_volumes(data);
  parse_metrics(data);

  return data;
}

void Parser::parse_basic_info(PrefetchData& data) {
  char buffer[1024];
  size_t size;

  // Парсинг имени исполняемого файла
  libscca_file_get_utf8_executable_filename_size(scca_file_, &size, nullptr);
  libscca_file_get_utf8_executable_filename(
      scca_file_, reinterpret_cast<uint8_t*>(buffer), size, nullptr);
  data.set_executable_name(buffer);

  // Получение хэша и счетчика запусков
  uint32_t hash, count;
  libscca_file_get_prefetch_hash(scca_file_, &hash, nullptr);
  libscca_file_get_run_count(scca_file_, &count, nullptr);
  data.set_prefetch_hash(hash);
  data.set_run_count(count);
}

/**
 * @brief Конвертирует Windows FILETIME в Unix time_t с проверкой валидности
 * @param filetime Время в формате FILETIME (100-нс интервалы с 1601-01-01)
 * @return Время в Unix-формате или 0 для невалидных дат
 */
time_t Parser::convert_filetime_safe(uint64_t filetime) {
  const uint64_t FILETIME_ZERO = 0;
  const uint64_t FILETIME_MAX = 2650467744000000000ULL;  // ~30828 год

  if (filetime <= FILETIME_ZERO || filetime > FILETIME_MAX) {
    return 0;
  }

  // Конвертация с учетом разницы эпох (1601-1970)
  return static_cast<time_t>((filetime - 116444736000000000ULL) / 10000000ULL);
}

/**
 * @brief Форматирует время в строку с проверкой валидности
 * @param time Время в Unix-формате
 * @return Строка с датой или "Not available" для невалидных дат
 */
std::string Parser::format_time_safe(time_t time) {
  if (time <= 0) {
    return "Not available";
  }

  char buffer[80];
  std::tm* tm_info = std::localtime(&time);
  if (!tm_info) {
    return "Invalid time";
  }

  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  return std::string(buffer);
}

void Parser::parse_run_history(PrefetchData& data) {
  for (int i = 0;; ++i) {
    uint64_t ft;
    if (libscca_file_get_last_run_time(scca_file_, i, &ft, nullptr) != 1)
      break;

    time_t unix_time = convert_filetime_safe(ft);
    if (unix_time > 0) {
      data.add_run_time(unix_time);
    }
  }
}

void Parser::parse_volumes(PrefetchData& data) {
  int count;
  libscca_file_get_number_of_volumes(scca_file_, &count, nullptr);

  for (int i = 0; i < count; ++i) {
    libscca_volume_information_t* vol;
    libscca_file_get_volume_information(scca_file_, i, &vol, nullptr);

    char path[256];
    size_t path_size;
    libscca_volume_information_get_utf8_device_path_size(vol, &path_size,
                                                         nullptr);
    libscca_volume_information_get_utf8_device_path(
        vol, reinterpret_cast<uint8_t*>(path), path_size, nullptr);

    uint32_t serial;
    uint64_t create_time;
    libscca_volume_information_get_serial_number(vol, &serial, nullptr);
    libscca_volume_information_get_creation_time(vol, &create_time, nullptr);

    data.add_volume(VolumeInfo(path, serial, create_time));
    libscca_volume_information_free(&vol, nullptr);
  }
}

void Parser::parse_metrics(PrefetchData& data) {
  int count;
  libscca_file_get_number_of_file_metrics_entries(scca_file_, &count, nullptr);

  for (int i = 0; i < count; ++i) {
    libscca_file_metrics_t* metric;
    libscca_file_get_file_metrics_entry(scca_file_, i, &metric, nullptr);

    char name[512];
    size_t name_size;
    libscca_file_metrics_get_utf8_filename_size(metric, &name_size, nullptr);
    libscca_file_metrics_get_utf8_filename(
        metric, reinterpret_cast<uint8_t*>(name), name_size, nullptr);

    uint64_t ref;
    libscca_file_metrics_get_file_reference(metric, &ref, nullptr);

    data.add_metric(FileMetric(name, ref));
    libscca_file_metrics_free(&metric, nullptr);
  }
}

}  // namespace PrefetchAnalysis
