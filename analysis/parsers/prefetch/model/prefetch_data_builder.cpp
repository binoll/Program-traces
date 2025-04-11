/**
 * @file prefetch_data.cpp
 * @brief Реализация интерфейсов для построения объектов PrefetchData с валидацией и сборкой данных.
*/

#include "prefetch_data_builder.hpp"
#include <algorithm>
#include "iprefetch_data.hpp"
#include "prefetch_data.hpp"

namespace PrefetchAnalysis {

PrefetchDataBuilder& PrefetchDataBuilder::setExecutableName(std::string name) {
  constexpr const char* forbidden = R"(\/:*?"<>|)";
  if (name.empty()) {
    throw InvalidExecutableNameException("Имя файла не может быть пустым");
  }
  if (name.find_first_of(forbidden) != std::string::npos) {
    throw InvalidExecutableNameException(
        "Недопустимые символы в имени файла: " + name);
  }
  executable_name_ = std::move(name);
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::setPrefetchHash(
    uint32_t hash) noexcept {
  prefetch_hash_ = hash;
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::setRunCount(uint32_t count) noexcept {
  run_count_ = count;
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::addRunTime(time_t time) noexcept {
  if (run_times_.size() < 8) {
    run_times_.push_back(time);
  }
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::addVolume(VolumeInfo vol) {
  volumes_.emplace_back(std::move(vol));
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::addMetric(FileMetric metric) {
  metrics_.emplace_back(std::move(metric));
  return *this;
}

std::unique_ptr<IPrefetchData> PrefetchDataBuilder::build() {
  if (executable_name_.empty()) {
    throw InvalidExecutableNameException("Не указано имя исполняемого файла");
  }
  return std::make_unique<PrefetchData>(
      std::move(executable_name_), prefetch_hash_, run_count_,
      std::move(run_times_), std::move(volumes_), std::move(metrics_));
}

}
