#include "file_metric.hpp"

namespace PrefetchAnalysis {

bool is_valid(const std::string& path) noexcept {
  return !path.empty() && path.find_first_not_of(' ') != std::string::npos;
}

FileMetric::FileMetric(std::string filename, uint64_t mft_ref,
                       uint64_t file_size, uint32_t access_flags,
                       uint64_t last_access_time)
    : filename_(std::move(filename)),
      file_reference_(mft_ref),
      file_size_(file_size),
      access_flags_(access_flags),
      last_access_time_(last_access_time) {
  if (!is_valid(filename_)) {
    throw InvalidFileMetricException("Указан неверный путь к файлу");
  }
}

const std::string& FileMetric::getFilename() const noexcept {
  return filename_;
}

uint64_t FileMetric::getFileReference() const noexcept {
  return file_reference_;
}

uint64_t FileMetric::getFileSize() const noexcept { return file_size_; }

uint32_t FileMetric::getAccessFlags() const noexcept { return access_flags_; }

uint64_t FileMetric::getLastAccessTime() const noexcept {
  return last_access_time_;
}

bool FileMetric::wasAccessedForRead() const noexcept {
  return (access_flags_ & FILE_METRIC_ACCESS_READ) != 0;
}

bool FileMetric::wasAccessedForWrite() const noexcept {
  return (access_flags_ & FILE_METRIC_ACCESS_WRITE) != 0;
}

bool FileMetric::wasExecuted() const noexcept {
  return (access_flags_ & FILE_METRIC_ACCESS_EXECUTE) != 0;
}

}
