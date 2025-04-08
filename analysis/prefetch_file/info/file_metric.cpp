/**
 * @file file_metric.cpp
 * @brief Реализация методов класса FileMetric
*/

#include <algorithm>
#include <cctype>
#include "file_metric.hpp"

namespace PrefetchAnalysis {

FileMetric::FileMetric(std::string name, uint64_t ref)
    : filename_(std::move(name)), file_reference_(ref) {
  // Проверка на пустое имя или состоящее только из пробелов
  if (filename_.empty()) {
    throw FileMetricException("Имя файла не может быть пустым");
  }

  if (std::ranges::all_of(filename_,
                  [](unsigned char c) { return std::isspace(c); })) {
    throw FileMetricException("Имя файла не может состоять только из пробелов");
  }
}

std::string FileMetric::filename() const noexcept {
  return filename_;
}

uint64_t FileMetric::file_reference() const noexcept {
  return file_reference_;
}

}
