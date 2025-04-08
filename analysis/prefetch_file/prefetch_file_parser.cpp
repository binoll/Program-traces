/**
 * @file prefetch_file_parser.cpp
 * @brief Реализация парсера Prefetch-файлов
 */

#include "prefetch_file_parser.hpp"
#include "../../logger/logger.hpp"
#include <sstream>
#include <stdexcept>

namespace PrefetchAnalysis {

constexpr uint64_t FILETIME_EPOCH_DIFF = 116444736000000000ULL;
constexpr uint64_t FILETIME_MAX_VALID = 2650467744000000000ULL;

PrefetchParser::PrefetchParser() : scca_handle_(nullptr) {
  try {
    initialize();
  } catch (const std::exception& e) {
    throw FileOpenException(std::string("Ошибка инициализации: ") + e.what());
  }
}

PrefetchParser::~PrefetchParser() noexcept {
  if (scca_handle_) {
    libscca_file_free(&scca_handle_, nullptr);
  }
}

void PrefetchParser::initialize() {
  if (libscca_file_initialize(&scca_handle_, nullptr) != 1) {
    throw FileOpenException("Ошибка инициализации libscca");
  }
}

PrefetchData PrefetchParser::parse(const std::string& path) const {
  if (libscca_file_open(scca_handle_, path.c_str(), LIBSCCA_ACCESS_FLAG_READ,
                        nullptr) != 1) {
    throw FileOpenException("Не удалось открыть файл: " + path);
  }

  PrefetchData data;
  try {
    parse_basic_info(data);
    parse_run_times(data);
    parse_volumes(data);
    parse_metrics(data);
  } catch (...) {
    libscca_file_close(scca_handle_, nullptr);
    throw;
  }

  libscca_file_close(scca_handle_, nullptr);
  return data;
}

time_t PrefetchParser::convert_filetime(uint64_t filetime) {
  if (filetime < FILETIME_EPOCH_DIFF || filetime > FILETIME_MAX_VALID) {
    std::ostringstream oss;
    oss << "Недопустимое значение FILETIME: 0x" << std::hex << filetime;
    throw InvalidTimestampException(oss.str());
  }
  return static_cast<time_t>((filetime - FILETIME_EPOCH_DIFF) / 10000000ULL);
}

void PrefetchParser::parse_basic_info(PrefetchData& data) const {
  char filename_buffer[256] = {0};
  size_t name_length = 0;

  if (libscca_file_get_utf8_executable_filename_size(scca_handle_, &name_length,
                                                     nullptr) != 1) {
    throw DataReadException("Не удалось получить размер имени файла");
  }

  if (name_length == 0 || name_length > sizeof(filename_buffer)) {
    throw InvalidFormatException("Некорректная длина имени файла");
  }

  if (libscca_file_get_utf8_executable_filename(
          scca_handle_, reinterpret_cast<uint8_t*>(filename_buffer),
          name_length, nullptr) != 1) {
    throw DataReadException("Ошибка чтения имени файла");
  }

  try {
    data.set_executable_name(filename_buffer);
  } catch (const InvalidExecutableNameException& e) {
    throw InvalidFormatException(e.what());
  }

  uint32_t prefetch_hash = 0;
  if (libscca_file_get_prefetch_hash(scca_handle_, &prefetch_hash, nullptr) !=
      1) {
    throw DataReadException("Ошибка чтения хеша файла");
  }
  data.set_prefetch_hash(prefetch_hash);

  uint32_t run_count = 0;
  if (libscca_file_get_run_count(scca_handle_, &run_count, nullptr) != 1) {
    throw DataReadException("Ошибка чтения счетчика запусков");
  }
  data.set_run_count(run_count);
}

void PrefetchParser::parse_run_times(PrefetchData& data) const {
  for (int i = 0;; ++i) {
    uint64_t ft = 0;
    if (libscca_file_get_last_run_time(scca_handle_, i, &ft, nullptr) != 1)
      break;

    try {
      data.add_run_time(convert_filetime(ft));
    } catch (const InvalidTimestampException& e) {
      LOGGER->debug(e.what());
    }
  }
}

void PrefetchParser::parse_volumes(PrefetchData& data) const {
  int count = 0;
  if (libscca_file_get_number_of_volumes(scca_handle_, &count, nullptr) != 1) {
    throw DataReadException("Ошибка чтения количества томов");
  }

  for (int i = 0; i < count; ++i) {
    libscca_volume_information_t* vol = nullptr;
    if (libscca_file_get_volume_information(scca_handle_, i, &vol, nullptr) !=
        1) {
      continue;
    }

    try {
      char path[256] = {0};
      size_t path_size = 0;

      if (libscca_volume_information_get_utf8_device_path_size(vol, &path_size,
                                                               nullptr) != 1 ||
          libscca_volume_information_get_utf8_device_path(
              vol, reinterpret_cast<uint8_t*>(path), path_size, nullptr) != 1) {
        throw InvalidDevicePathException("Ошибка чтения пути тома");
      }

      uint32_t serial = 0;
      uint64_t create_time = 0;
      if (libscca_volume_information_get_serial_number(vol, &serial, nullptr) !=
              1 ||
          libscca_volume_information_get_creation_time(vol, &create_time,
                                                       nullptr) != 1) {
        throw DataReadException("Ошибка чтения метаданных тома");
      }

      data.add_volume(VolumeInfo(path, serial, create_time));
    } catch (...) {
      libscca_volume_information_free(&vol, nullptr);
      throw;
    }
    libscca_volume_information_free(&vol, nullptr);
  }
}

void PrefetchParser::parse_metrics(PrefetchData& data) const {
  int count = 0;
  if (libscca_file_get_number_of_file_metrics_entries(scca_handle_, &count,
                                                      nullptr) != 1) {
    throw DataReadException("Ошибка чтения количества метрик");
  }

  for (int i = 0; i < count; ++i) {
    libscca_file_metrics_t* metric = nullptr;
    if (libscca_file_get_file_metrics_entry(scca_handle_, i, &metric,
                                            nullptr) != 1) {
      continue;
    }

    try {
      char name[512] = {0};
      size_t name_size = 0;

      if (libscca_file_metrics_get_utf8_filename_size(metric, &name_size,
                                                      nullptr) != 1 ||
          libscca_file_metrics_get_utf8_filename(
              metric, reinterpret_cast<uint8_t*>(name), name_size, nullptr) !=
              1) {
        throw InvalidFileMetricException("Ошибка чтения имени файла");
      }

      uint64_t ref = 0;
      if (libscca_file_metrics_get_file_reference(metric, &ref, nullptr) != 1) {
        throw InvalidFileMetricException("Ошибка чтения MFT-ссылки");
      }

      data.add_metric(FileMetric(name, ref));
    } catch (...) {
      libscca_file_metrics_free(&metric, nullptr);
      throw;
    }
    libscca_file_metrics_free(&metric, nullptr);
  }
}

}
