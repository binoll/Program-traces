/**
 * @file prefetch_parser.cpp
 * @brief Реализация парсера Prefetch-файлов.
*/

#include "prefetch_parser.hpp"
#include <sstream>
#include <stdexcept>
#include "../../../utils/logger/logger.hpp"

namespace PrefetchAnalysis {

constexpr uint64_t FILETIME_EPOCH_DIFF = 116444736000000000ULL;
constexpr uint64_t FILETIME_MAX_VALID = 2650467744000000000ULL;

PrefetchParser::PrefetchParser() : scca_handle_(nullptr) {
  LOGGER->info("Инициализация парсера Prefetch файлов...");
  LOGGER->debug("Инициализация библиотеки libscca...");
  if (libscca_file_initialize(&scca_handle_, nullptr) != 1) {
    throw InitLibError("libscca");
  }
  LOGGER->info("Инициализация libscca выполнена успешно");
}

PrefetchParser::~PrefetchParser() noexcept {
  if (scca_handle_) {
    libscca_file_free(&scca_handle_, nullptr);
    LOGGER->debug("Ресурсы для scca_handle_ освобождены");
  }
}

PrefetchDataBuilder PrefetchParser::parse(const std::string& path) const {
  LOGGER->info("Начинаем парсинг Prefetch файла: {}", path);

  if (libscca_file_open(scca_handle_, path.c_str(), LIBSCCA_ACCESS_FLAG_READ,
                        nullptr) != 1) {
    throw FileOpenException("Не удалось открыть файл: " + path);
  }

  LOGGER->debug("Файл {} открыт успешно", path);

  PrefetchDataBuilder data;
  try {
    parseBasicInfo(data);
    parseRunTimes(data);
    parseVolumes(data);
    parseMetrics(data);
  } catch (...) {
    libscca_file_close(scca_handle_, nullptr);
    throw;
  }

  libscca_file_close(scca_handle_, nullptr);
  LOGGER->info("Файл {} успешно закрыт", path);
  return data;
}

time_t PrefetchParser::convertFiletime(uint64_t filetime) {
  if (filetime < FILETIME_EPOCH_DIFF || filetime > FILETIME_MAX_VALID) {
    std::ostringstream oss;
    oss << "Недопустимое значение FILETIME: 0x" << std::hex << filetime;
    LOGGER->warn(oss.str());
    throw InvalidTimestampException(oss.str());
  }
  return static_cast<time_t>((filetime - FILETIME_EPOCH_DIFF) / 10000000ULL);
}

void PrefetchParser::parseBasicInfo(PrefetchDataBuilder& data) const {
  LOGGER->debug("Чтение базовой информации о Prefetch файле...");

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
    data.setExecutableName(filename_buffer);
    LOGGER->debug("Исполняемое имя файла: {}", filename_buffer);
  } catch (const InvalidExecutableNameException& e) {
    throw InvalidFormatException(e.what());
  }

  uint32_t prefetch_hash = 0;
  if (libscca_file_get_prefetch_hash(scca_handle_, &prefetch_hash, nullptr) !=
      1) {
    throw DataReadException("Ошибка чтения хеша файла");
  }
  data.setPrefetchHash(prefetch_hash);
  LOGGER->debug("Хеш Prefetch файла: 0x{:x}", prefetch_hash);

  uint32_t run_count = 0;
  if (libscca_file_get_run_count(scca_handle_, &run_count, nullptr) != 1) {
    throw DataReadException("Ошибка чтения счетчика запусков");
  }
  data.setRunCount(run_count);
  LOGGER->debug("Количество запусков: {}", run_count);
}

void PrefetchParser::parseRunTimes(PrefetchDataBuilder& data) const {
  LOGGER->debug("Чтение времен запусков...");

  for (int i = 0;; ++i) {
    uint64_t ft = 0;
    if (libscca_file_get_last_run_time(scca_handle_, i, &ft, nullptr) != 1) {
      LOGGER->debug("Нет больше времени запуска на индексе {}", i);
      break;
    }

    try {
      data.addRunTime(convertFiletime(ft));
      LOGGER->debug("Добавлено время запуска: {}", ft);
    } catch (const InvalidTimestampException& e) {
      LOGGER->debug("Неверный формат метки времени: {}", e.what());
    }
  }
}

void PrefetchParser::parseVolumes(PrefetchDataBuilder& data) const {
  LOGGER->debug("Чтение информации о томах...");

  int count = 0;
  if (libscca_file_get_number_of_volumes(scca_handle_, &count, nullptr) != 1) {
    throw DataReadException("Ошибка чтения количества томов");
  }

  LOGGER->debug("Количество томов: {}", count);

  for (int i = 0; i < count; ++i) {
    libscca_volume_information_t* vol = nullptr;
    if (libscca_file_get_volume_information(scca_handle_, i, &vol, nullptr) !=
        1) {
      LOGGER->warn("Не удалось получить информацию о томе на индексе {}", i);
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

      data.addVolume(VolumeInfo(path, serial, create_time));
      LOGGER->debug("Добавлен том с путем: {}", path);
    } catch (...) {
      libscca_volume_information_free(&vol, nullptr);
      throw;
    }
    libscca_volume_information_free(&vol, nullptr);
  }
}

void PrefetchParser::parseMetrics(PrefetchDataBuilder& data) const {
  LOGGER->debug("Чтение метрик файлов...");

  int count = 0;
  if (libscca_file_get_number_of_file_metrics_entries(scca_handle_, &count,
                                                      nullptr) != 1) {
    throw DataReadException("Ошибка чтения количества метрик");
  }

  LOGGER->debug("Количество метрик: {}", count);

  for (int i = 0; i < count; ++i) {
    libscca_file_metrics_t* metric = nullptr;
    if (libscca_file_get_file_metrics_entry(scca_handle_, i, &metric,
                                            nullptr) != 1) {
      LOGGER->warn("Не удалось получить метрику на индексе {}", i);
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

      data.addMetric(FileMetric(name, ref));
      LOGGER->debug("Добавлена метрика для файла: {}", name);
    } catch (...) {
      libscca_file_metrics_free(&metric, nullptr);
      throw;
    }
    libscca_file_metrics_free(&metric, nullptr);
  }
}

}
