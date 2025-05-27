#include "file_metric.hpp"

#include <utility>

namespace PrefetchAnalysis {

FileMetric::FileMetric(fs::path filename, const uint64_t mft_ref,
                       const uint64_t file_size, const uint32_t access_flags,
                       const uint64_t last_access_time)
    : filename_(std::move(filename)),
      file_reference_(mft_ref),
      file_size_(file_size),
      access_flags_(access_flags),
      last_access_time_(last_access_time) {}

const fs::path& FileMetric::getFilename() const noexcept { return filename_; }

uint64_t FileMetric::getFileReference() const noexcept {
  return file_reference_;
}

uint64_t FileMetric::getFileSize() const noexcept { return file_size_; }

uint32_t FileMetric::getAccessFlags() const noexcept { return access_flags_; }

uint64_t FileMetric::getLastAccessTime() const noexcept {
  return last_access_time_;
}

template <FileMetricAccess flag>
[[nodiscard]] bool FileMetric::checkAccessFlags() const noexcept {
  return (access_flags_ & static_cast<uint32_t>(flag)) != 0;
}

[[nodiscard]] bool FileMetric::checkAccessFlag(uint32_t types) const noexcept {
  return (access_flags_ & types) != 0;
}

void FileMetric::validate() const noexcept {
  constexpr uint64_t min_valid_time =
      116444736000000000ULL;  // 01.01.1601 (UTC)

  if (!validatePath(filename_)) {
    throw InvalidFileMetricException(filename_.string());
  }

  if (file_reference_ == 0) {
    throw InvalidFileMetricException(filename_.string(), file_reference_);
  }

  if (last_access_time_ < min_valid_time) {
    throw InvalidFileMetricException(filename_.string(),
                                     std::to_string(last_access_time_));
  }
}

bool FileMetric::validatePath(const fs::path& path) {
  constexpr std::string_view invalid_chars = "<>:\"|?*";

  if (path.empty()) {
    return false;
  }

  return path.string().find_first_of(invalid_chars) == std::string::npos;
}

}
