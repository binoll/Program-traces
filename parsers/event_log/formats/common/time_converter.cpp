#include "time_converter.hpp"

namespace EventLogAnalysis {

std::chrono::system_clock::time_point TimeConverter::filetimeToSystemTime(
    uint64_t filetime) {
  // FILETIME is 100-nanosecond intervals since January 1, 1601 (UTC)
  // Unix epoch is January 1, 1970 (UTC)
  // Difference is 116444736000000000 * 100ns

  if (filetime < FILETIME_EPOCH_DIFFERENCE) {
    // Return epoch if filetime is before epoch (or invalid/zero)
    return std::chrono::system_clock::from_time_t(0);
  }

  uint64_t intervals_since_epoch = filetime - FILETIME_EPOCH_DIFFERENCE;
  uint64_t ns_since_epoch = intervals_since_epoch * 100;

  auto duration = std::chrono::nanoseconds(ns_since_epoch);
  return std::chrono::system_clock::time_point(
      std::chrono::duration_cast<std::chrono::system_clock::duration>(
          duration));
}

uint64_t TimeConverter::unixTimeToFiletime(time_t unix_time) {
  uint64_t intervals = static_cast<uint64_t>(unix_time) * 10000000ULL;
  return intervals + FILETIME_EPOCH_DIFFERENCE;
}

uint64_t TimeConverter::systemTimeToFiletime(
    const std::chrono::system_clock::time_point& tp) {
  auto duration = tp.time_since_epoch();
  auto ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

  // Convert ns to 100-ns intervals
  uint64_t intervals = ns / 100;
  return intervals + FILETIME_EPOCH_DIFFERENCE;
}

uint64_t TimeConverter::secondsSince1970ToFiletime(uint64_t seconds) {
  return seconds * 10000000ULL + FILETIME_EPOCH_DIFFERENCE;
}

}
