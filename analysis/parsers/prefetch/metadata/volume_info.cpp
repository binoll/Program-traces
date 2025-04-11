/**
 * @file volume_info.cpp
 * @brief Реализация классов для работы с метаданными логических томов
 */

#include "volume_info.hpp"

namespace PrefetchAnalysis {

bool is_valid(const std::string& path) noexcept {
  return !path.empty() && path.find_first_not_of(' ') != std::string::npos;
}

VolumeInfo::VolumeInfo(std::string device_path, uint32_t serial,
                       uint64_t create_time, uint64_t size, uint32_t type)
    : device_path_(std::move(device_path)),
      serial_number_(serial),
      creation_time_(create_time),
      volume_size_(size),
      volume_type_(type) {
  if (!is_valid(device_path_)) {
    throw InvalidDevicePathException("Неверный путь к устройству NT");
  }
}

const std::string& VolumeInfo::getDevicePath() const noexcept {
  return device_path_;
}

uint32_t VolumeInfo::getSerialNumber() const noexcept { return serial_number_; }

uint64_t VolumeInfo::getCreationTime() const noexcept { return creation_time_; }

uint64_t VolumeInfo::getVolumeSize() const noexcept { return volume_size_; }

uint32_t VolumeInfo::getVolumeType() const noexcept { return volume_type_; }

bool VolumeInfo::isRemovable() const noexcept {
  return (volume_type_ & VOLUME_TYPE_REMOVABLE) != 0;
}

bool VolumeInfo::isNetworkDrive() const noexcept {
  return (volume_type_ & VOLUME_TYPE_NETWORK) != 0;
}

}
