#include "volume_info.hpp"

#include <algorithm>
#include <array>
#include <sstream>
#include <string_view>

#include "../../../exceptions/prefetch/volume_exception.hpp"

namespace PrefetchAnalysis {

VolumeInfo::VolumeInfo(std::string device_path, const uint32_t serial_number,
                       const uint64_t creation_time, const uint64_t volume_size,
                       const uint32_t volume_type)
    : device_path_(std::move(device_path)),
      serial_number_(serial_number),
      creation_time_(creation_time),
      volume_size_(volume_size),
      volume_type_(volume_type) {}

const std::string& VolumeInfo::getDevicePath() const noexcept {
  return device_path_;
}

uint32_t VolumeInfo::getSerialNumber() const noexcept { return serial_number_; }

uint64_t VolumeInfo::getCreationTime() const noexcept { return creation_time_; }

uint64_t VolumeInfo::getVolumeSize() const noexcept { return volume_size_; }

uint32_t VolumeInfo::getVolumeType() const noexcept { return volume_type_; }

template <VolumeType type>
bool VolumeInfo::checkVolumeType() const noexcept {
  return (volume_type_ & static_cast<uint32_t>(type)) != 0;
}

bool VolumeInfo::checkVolumeTypes(uint32_t types) const noexcept {
  return (volume_type_ & types) != 0;
}

void VolumeInfo::validate() const {
  constexpr uint64_t min_valid_time =
      116444736000000000ULL;  // 01-01-1601 в FILETIME

  if (!validatePath()) {
    throw InvalidVolumeException(device_path_);
  }

  if (creation_time_ <= min_valid_time) {
    throw InvalidVolumeException(device_path_, creation_time_);
  }

  if (!validateVolumeType()) {
    throw InvalidVolumeException(device_path_, volume_type_);
  }

  if (serial_number_ == 0) {
    throw InvalidVolumeException(device_path_, std::to_string(serial_number_));
  }
}

bool VolumeInfo::validatePath() const {
  constexpr std::array<std::string_view, 6> valid_keywords = {
      "HarddiskVolume", "CdRom", "LanmanRedirector",
      "Volume{",        "VHD",   "CloudFS"};

  if (constexpr std::string_view device_prefix = "\\Device\\";
      !device_path_.starts_with(device_prefix)) {
    return false;
  }

  return std::ranges::any_of(valid_keywords, [this](auto&& keyword) {
    return device_path_.find(keyword) != std::string::npos;
  });
}

bool VolumeInfo::validateVolumeType() const noexcept {
  constexpr uint32_t valid_mask = static_cast<uint32_t>(VolumeType::FIXED) |
                                  static_cast<uint32_t>(VolumeType::REMOVABLE) |
                                  static_cast<uint32_t>(VolumeType::NETWORK) |
                                  static_cast<uint32_t>(VolumeType::OPTICAL) |
                                  static_cast<uint32_t>(VolumeType::RAMDISK) |
                                  static_cast<uint32_t>(VolumeType::VIRTUAL) |
                                  static_cast<uint32_t>(VolumeType::SYSTEM) |
                                  static_cast<uint32_t>(VolumeType::CLOUD) |
                                  static_cast<uint32_t>(VolumeType::ENCRYPTED) |
                                  static_cast<uint32_t>(VolumeType::TEMPORARY);

  return (volume_type_ & ~valid_mask) == 0;
}

}
