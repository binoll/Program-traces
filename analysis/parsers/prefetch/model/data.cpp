#include "data.hpp"

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace PrefetchAnalysis {

PrefetchData::PrefetchData(PrefetchDataStorage&& storage)
    : storage_(std::move(storage)) {
  if (storage_.executable_name.empty()) {
    throw PrefetchDataException("Недопустимое имя исполняемого файла");
  }
}

std::string PrefetchData::getExecutableName() const noexcept {
  return storage_.executable_name;
}

uint32_t PrefetchData::getPrefetchHash() const noexcept {
  return storage_.prefetch_hash;
}

uint32_t PrefetchData::getRunCount() const noexcept {
  return storage_.run_count;
}

const std::vector<time_t>& PrefetchData::getRunTimes() const noexcept {
  return storage_.run_times;
}

time_t PrefetchData::getLastRunTime() const noexcept {
  return storage_.last_run_time;
}

const std::vector<VolumeInfo>& PrefetchData::getVolumes() const noexcept {
  return storage_.volumes;
}

VolumeInfo PrefetchData::getMainVolume() const {
  return storage_.volumes.front();
}

const std::vector<FileMetric>& PrefetchData::getMetrics() const noexcept {
  return storage_.metrics;
}

std::vector<FileMetric> PrefetchData::getDllMetrics() const {
  std::vector<FileMetric> result;
  std::ranges::copy_if(
      storage_.metrics, std::back_inserter(result),
      [](const FileMetric& metric) {
        const auto ext = fs::path(metric.getFilename()).extension().string();
        return ext == ".dll" || ext == ".DLL";
      });
  return result;
}

uint8_t PrefetchData::getFormatVersion() const noexcept {
  return storage_.format_version;
}

WindowsVersion PrefetchData::getWindowsVersion() const noexcept {
  switch (storage_.format_version) {
    case 10:
      return WindowsVersion::WIN_XP;
    case 11:
      return WindowsVersion::WIN_XP_SP2;
    case 12:
      return WindowsVersion::WIN_XP_SP3;
    case 17:
      return WindowsVersion::WIN_VISTA;
    case 18:
      return WindowsVersion::WIN_VISTA_SP1;
    case 19:
      return WindowsVersion::WIN_VISTA_SP2;
    case 23:
      return WindowsVersion::WIN7;
    case 24:
      return WindowsVersion::WIN7_SP1;
    case 26:
      return WindowsVersion::WIN8;
    case 30:
      return WindowsVersion::WIN8_1;
    case 31:
      return WindowsVersion::WIN10_1507;
    case 32:
      return WindowsVersion::WIN10_1511;
    case 33:
      return WindowsVersion::WIN10_1607;
    case 34:
      return WindowsVersion::WIN10_1703;
    case 35:
      return WindowsVersion::WIN10_1709;
    case 36:
      return WindowsVersion::WIN10_1803;
    case 37:
      return WindowsVersion::WIN10_1809;
    case 38:
      return WindowsVersion::WIN10_1903;
    case 39:
      return WindowsVersion::WIN10_1909;
    case 40:
      return WindowsVersion::WIN10_2004;
    case 41:
      return WindowsVersion::WIN10_20H2;
    case 42:
      return WindowsVersion::WIN10_21H1;
    case 43:
      return WindowsVersion::WIN10_21H2;
    case 44:
      return WindowsVersion::WIN11_21H2;
    case 45:
      return WindowsVersion::WIN11_22H2;
    case 46:
      return WindowsVersion::WIN11_23H2;
    case 47:
      return WindowsVersion::WIN11_24H2;
    default:
      return WindowsVersion::UNKNOWN;
  }
}
std::string PrefetchData::getWindowsVersionString() const noexcept {
  return std::string(WindowsVersionToString(getWindowsVersion()));
}

}
