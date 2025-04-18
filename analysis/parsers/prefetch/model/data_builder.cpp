#include "data_builder.hpp"

#include <algorithm>

#include "data.hpp"
#include "idata.hpp"

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
  storage_.executable_name = std::move(name);
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::setPrefetchHash(
    uint32_t hash) noexcept {
  storage_.prefetch_hash = hash;
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::setRunCount(uint32_t count) noexcept {
  storage_.run_count = count;
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::addRunTime(time_t time) noexcept {
  if (storage_.run_times.size() < 8) {
    storage_.run_times.push_back(time);
  }
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::addVolume(const VolumeInfo& vol) {
  if (!vol.isValid()) {
    throw PrefetchDataException("Невалидные данные тома");
  }
  storage_.volumes.push_back(vol);
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::addMetric(const FileMetric& metric) {
  if (!metric.isValid()) {
    throw PrefetchDataException("Невалидные данные метрики");
  }
  storage_.metrics.push_back(metric);
  return *this;
}

std::unique_ptr<IPrefetchData> PrefetchDataBuilder::build() {
  if (storage_.executable_name.empty()) {
    throw InvalidExecutableNameException("Не указано имя исполняемого файла");
  }
  if (storage_.run_count != storage_.run_times.size()) {
    throw PrefetchDataException(
        "Несоответствие количества запусков и временных меток");
  }
  return std::make_unique<PrefetchData>(std::move(storage_));
}

PrefetchDataBuilder& PrefetchDataBuilder::setFormatVersion(
    uint8_t format_version) noexcept {
  storage_.format_version = format_version;
  return *this;
}

PrefetchDataBuilder& PrefetchDataBuilder::setLastRunTime(
    time_t last_run_time) noexcept {
  storage_.last_run_time = last_run_time;
  return *this;
}

}
