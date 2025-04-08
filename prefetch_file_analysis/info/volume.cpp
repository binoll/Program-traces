/**
 * @file volume.cpp
 * @brief Реализация методов класса VolumeInfo с обработкой исключений
*/

#include <algorithm>
#include <cctype>
#include "volume.hpp"

namespace PrefetchAnalysis {

VolumeInfo::VolumeInfo(std::string device_path, uint32_t serial,
                       uint64_t create_time)
    : device_path_(std::move(device_path)),
      serial_number_(serial),
      creation_time_(create_time) {
  // Проверка на пустую строку или строку из пробелов
  bool is_invalid = device_path_.empty() ||
                    std::ranges::all_of(device_path_, [](unsigned char c) {
                      return ::isspace(c);
                    });

  if (is_invalid) {
    throw InvalidDevicePathException(device_path_);
  }
}

std::string VolumeInfo::device_path() const noexcept {
  return device_path_;
}

uint32_t VolumeInfo::serial_number() const noexcept {
  return serial_number_;
}

uint64_t VolumeInfo::creation_time() const noexcept {
  return creation_time_;
}

}
