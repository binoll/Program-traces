/**
 * @file volume_info.cpp
 * @brief Реализация классов для работы с метаданными логических томов.
*/

#include "volume_info.hpp"
#include <algorithm>

namespace PrefetchAnalysis {

bool is_valid(const std::string& path) noexcept {
  return !path.empty() && !std::ranges::all_of(path, ::isspace);
}

VolumeInfo::VolumeInfo(std::string device_path, uint32_t serial,
                       uint64_t create_time)
    : device_path_(std::move(device_path)),
      serial_number_(serial),
      creation_time_(create_time) {
  if (!is_valid(device_path_)) {
    throw InvalidDevicePathException(device_path_);
  }
}

const std::string& VolumeInfo::getDevicePath() const noexcept {
  return device_path_;
}

uint32_t VolumeInfo::getSerialNumber() const noexcept {
  return serial_number_;
}

uint64_t VolumeInfo::getCreationTime() const noexcept {
  return creation_time_;
}

}
