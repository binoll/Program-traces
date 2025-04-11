/**
 * @file prefetch_data.cpp
 * @brief Реализация интерфейсов для работы с данными Prefetch-файлов.
*/

#include "prefetch_data.hpp"
#include <algorithm>

namespace PrefetchAnalysis {
PrefetchData::PrefetchData(std::string name, uint32_t hash, uint32_t count,
                           std::vector<time_t> times,
                           std::vector<VolumeInfo> vols,
                           std::vector<FileMetric> mets)
    : executable_name_(std::move(name)),
      prefetch_hash_(hash),
      run_count_(count),
      run_times_(std::move(times)),
      volumes_(std::move(vols)),
      metrics_(std::move(mets)) {}

std::string PrefetchData::getExecutableName() const noexcept {
  return executable_name_;
}

uint32_t PrefetchData::getPrefetchHash() const noexcept {
  return prefetch_hash_;
}

uint32_t PrefetchData::getRunCount() const noexcept {
  return run_count_;
}

const std::vector<time_t>& PrefetchData::getRunTimes() const noexcept {
  return run_times_;
}

const std::vector<VolumeInfo>& PrefetchData::getVolumes() const noexcept {
  return volumes_;
}

const std::vector<FileMetric>& PrefetchData::getMetrics() const noexcept {
  return metrics_;
}

}
