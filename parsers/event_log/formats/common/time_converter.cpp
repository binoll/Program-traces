#include "time_converter.hpp"

namespace EventLogAnalysis {

inline std::chrono::system_clock::time_point
TimeConverter::filetimeToSystemTime(uint64_t filetime) {
  if (filetime <= FILETIME_EPOCH_DIFFERENCE) {
    return std::chrono::system_clock::time_point{};
  }

  const uint64_t ns_since_epoch =
      (filetime - FILETIME_EPOCH_DIFFERENCE) * NS_PER_FILETIME_TICK;
  return std::chrono::system_clock::time_point{
      std::chrono::duration_cast<std::chrono::system_clock::duration>(
          std::chrono::nanoseconds(ns_since_epoch))};
}

inline uint64_t TimeConverter::unixTimeToFiletime(time_t unix_time) {
  return static_cast<uint64_t>(unix_time) * HUNDRED_NS_PER_SECOND +
         FILETIME_EPOCH_DIFFERENCE;
}

inline uint64_t TimeConverter::systemTimeToFiletime(
    const std::chrono::system_clock::time_point& tp) {
  const auto duration = tp.time_since_epoch();
  const auto ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
  return ns / NS_PER_FILETIME_TICK + FILETIME_EPOCH_DIFFERENCE;
}

inline uint64_t TimeConverter::secondsSince1970ToFiletime(uint64_t seconds) {
  return seconds * HUNDRED_NS_PER_SECOND + FILETIME_EPOCH_DIFFERENCE;
}

}
