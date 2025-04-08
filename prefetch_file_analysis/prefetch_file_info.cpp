/**
 * @file prefetch_file_info.cpp
 * @brief Реализация методов класса PrefetchData
*/

#include "prefetch_file_info.hpp"
#include <algorithm>

namespace PrefetchAnalysis {

std::string PrefetchData::executable_name() const noexcept {
  return executable_name_;
}

uint32_t PrefetchData::prefetch_hash() const noexcept {
  return prefetch_hash_;
}

uint32_t PrefetchData::run_count() const noexcept {
  return run_count_;
}

const std::vector<time_t>& PrefetchData::run_times() const noexcept {
  return run_times_;
}

const std::vector<VolumeInfo>& PrefetchData::volumes() const noexcept {
  return volumes_;
}

const std::vector<FileMetric>& PrefetchData::metrics() const noexcept {
  return metrics_;
}

/*----------------------------------------------------------------------------*/
void PrefetchData::set_executable_name(std::string name) {
  if (name.empty()) {
    throw InvalidExecutableNameException("Пустое имя файла");
  }

  if (name.find_first_of("\\/:*?\"<>|") != std::string::npos) {
    throw InvalidExecutableNameException("Запрещенные символы в имени");
  }

  executable_name_ = std::move(name);
}

void PrefetchData::set_prefetch_hash(uint32_t hash) noexcept {
  prefetch_hash_ = hash;
}

void PrefetchData::set_run_count(uint32_t count) noexcept {
  run_count_ = std::min(count, 30u);  // Ограничение Windows
}

void PrefetchData::add_run_time(time_t time) noexcept {
  if (run_times_.size() < 8) {  // Сохраняем только последние 8 запусков
    run_times_.push_back(time);
  }
}

void PrefetchData::add_volume(VolumeInfo vol) {
  volumes_.push_back(std::move(vol));
}

void PrefetchData::add_metric(FileMetric metric) {
  metrics_.push_back(std::move(metric));
}

}
