/**
 * @file file_metric.cpp
 * @brief Реализация классов для работы с файловыми метриками NTFS.
*/

#include "file_metric.hpp"

namespace PrefetchAnalysis {

static bool is_valid(const std::string& path) noexcept {
  return !path.empty() && path.find_first_not_of(' ') != std::string::npos;
}

FileMetric::FileMetric(std::string filename, uint64_t mft_ref)
    : filename_(std::move(filename)), file_reference_(mft_ref) {
  if (!is_valid(filename_)) {
    throw InvalidFileMetricException("Некорректный путь к файлу: " + filename_);
  }
}

const std::string& FileMetric::getFilename() const noexcept {
  return filename_;
}

uint64_t FileMetric::getFileReference() const noexcept {
  return file_reference_;
}

}
