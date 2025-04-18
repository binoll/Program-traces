#include "parser.hpp"

#include <sstream>
#include <stdexcept>

#include "../../../../utils/logger/logger.hpp"

namespace PrefetchAnalysis {

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
    LOGGER->debug("Ресурсы libscca освобождены");
  }
}

std::unique_ptr<IPrefetchData> PrefetchParser::parse(
    const std::string& path) const {
  LOGGER->info("Начало обработки файла: {}", path);

  if (libscca_file_open(scca_handle_, path.c_str(), LIBSCCA_ACCESS_FLAG_READ,
                        nullptr) != 1) {
    throw FileOpenException("Не удалось открыть файл: " + path);
  }
  LOGGER->debug("Файл успешно открыт");

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
  LOGGER->info("Обработка файла завершена: {}", path);
  return data.build();
}

void PrefetchParser::parseBasicInfo(PrefetchDataBuilder& data) const {
  LOGGER->debug("Извлечение базовой информации...");

  char filename_buffer[256] = {0};
  size_t name_length = 0;

  if (libscca_file_get_utf8_executable_filename_size(scca_handle_, &name_length,
                                                     nullptr) != 1) {
    throw DataReadException("Ошибка определения размера имени файла");
  }

  if (name_length == 0 || name_length > sizeof(filename_buffer)) {
    throw InvalidFormatException("Некорректная длина имени файла");
  }

  if (libscca_file_get_utf8_executable_filename(
          scca_handle_, reinterpret_cast<uint8_t*>(filename_buffer),
          name_length, nullptr) != 1) {
    throw DataReadException("Ошибка чтения имени файла");
  }
  data.setExecutableName(filename_buffer);

  uint32_t prefetch_hash = 0;
  if (libscca_file_get_prefetch_hash(scca_handle_, &prefetch_hash, nullptr) !=
      1) {
    throw DataReadException("Ошибка чтения хеша");
  }
  data.setPrefetchHash(prefetch_hash);

  uint32_t run_count = 0;
  if (libscca_file_get_run_count(scca_handle_, &run_count, nullptr) != 1) {
    throw DataReadException("Ошибка чтения счетчика запусков");
  }
  data.setRunCount(run_count);

  uint32_t format_version = 0;
  if (libscca_file_get_format_version(scca_handle_, &format_version, nullptr) !=
      1) {
    throw DataReadException("Ошибка чтения версии формата");
  }
  data.setFormatVersion(format_version);
}

void PrefetchParser::parseRunTimes(PrefetchDataBuilder& data) const {
  LOGGER->debug("Извлечение временных меток запусков...");
  std::vector<uint64_t> valid_times;

  for (uint32_t i = 0;; ++i) {
    uint64_t ft = 0;
    if (libscca_file_get_last_run_time(scca_handle_, i, &ft, nullptr) != 1) {
      break;
    }

    if (ft == 0) continue;

    try {
      const time_t unix_time = convertFiletime(ft);
      data.addRunTime(unix_time);
      valid_times.push_back(ft);
    } catch (const InvalidTimestampException& e) {
      LOGGER->warn("Некорректная метка времени: {}", e.what());
    }
  }

  if (!valid_times.empty()) {
    data.setLastRunTime(convertFiletime(
        *std::max_element(valid_times.begin(), valid_times.end())));
  }
}

void PrefetchParser::parseVolumes(PrefetchDataBuilder& data) const {
  LOGGER->debug("Извлечение информации о томах...");

  int count = 0;
  if (libscca_file_get_number_of_volumes(scca_handle_, &count, nullptr) != 1) {
    throw DataReadException("Ошибка чтения количества томов");
  }

  for (int i = 0; i < count; ++i) {
    libscca_volume_information_t* vol = nullptr;
    if (libscca_file_get_volume_information(scca_handle_, i, &vol, nullptr) !=
        1) {
      LOGGER->warn("Ошибка чтения тома {}", i);
      continue;
    }

    char path[256] = {0};
    size_t path_size = 0;
    if (libscca_volume_information_get_utf8_device_path_size(vol, &path_size,
                                                             nullptr) != 1 ||
        libscca_volume_information_get_utf8_device_path(
            vol, reinterpret_cast<uint8_t*>(path), path_size, nullptr) != 1) {
      libscca_volume_information_free(&vol, nullptr);
      throw InvalidDevicePathException("Ошибка чтения пути тома");
    }

    uint32_t serial = 0;
    uint64_t create_time = 0;
    if (libscca_volume_information_get_serial_number(vol, &serial, nullptr) !=
            1 ||
        libscca_volume_information_get_creation_time(vol, &create_time,
                                                     nullptr) != 1) {
      libscca_volume_information_free(&vol, nullptr);
      throw DataReadException("Ошибка чтения метаданных тома");
    }

    data.addVolume(VolumeInfo(path, serial, create_time));
    libscca_volume_information_free(&vol, nullptr);
  }
}

void PrefetchParser::parseMetrics(PrefetchDataBuilder& data) const {
  LOGGER->debug("Извлечение файловых метрик...");

  int count = 0;
  if (libscca_file_get_number_of_file_metrics_entries(scca_handle_, &count,
                                                      nullptr) != 1) {
    throw DataReadException("Ошибка чтения количества метрик");
  }

  for (int i = 0; i < count; ++i) {
    libscca_file_metrics_t* metric = nullptr;
    if (libscca_file_get_file_metrics_entry(scca_handle_, i, &metric,
                                            nullptr) != 1) {
      LOGGER->warn("Ошибка чтения метрики {}", i);
      continue;
    }

    char name[512] = {0};
    size_t name_size = 0;
    if (libscca_file_metrics_get_utf8_filename_size(metric, &name_size,
                                                    nullptr) != 1 ||
        libscca_file_metrics_get_utf8_filename(metric,
                                               reinterpret_cast<uint8_t*>(name),
                                               name_size, nullptr) != 1) {
      libscca_file_metrics_free(&metric, nullptr);
      throw InvalidFileMetricException("Ошибка чтения имени файла");
    }

    uint64_t ref = 0;
    if (libscca_file_metrics_get_file_reference(metric, &ref, nullptr) != 1) {
      libscca_file_metrics_free(&metric, nullptr);
      throw InvalidFileMetricException("Ошибка чтения MFT-ссылки");
    }

    data.addMetric(FileMetric(name, ref));
    libscca_file_metrics_free(&metric, nullptr);
  }
}

time_t PrefetchParser::convertFiletime(uint64_t filetime) {
  if (filetime < FILETIME_EPOCH_DIFF || filetime > FILETIME_MAX_VALID) {
    std::ostringstream oss;
    oss << "Некорректное FILETIME: 0x" << std::hex << filetime;
    throw InvalidTimestampException(oss.str());
  }
  return static_cast<time_t>((filetime - FILETIME_EPOCH_DIFF) / 10000000ULL);
}

}
