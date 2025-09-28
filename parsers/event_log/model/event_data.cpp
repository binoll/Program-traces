#include "event_data.hpp"

#include "event_data_builder.hpp"

namespace EventLogAnalysis {

uint32_t EventData::event_id() const noexcept { return event_id_; }

uint64_t EventData::timestamp() const noexcept { return timestamp_; }

EventLevel EventData::level() const noexcept { return level_; }

const std::string& EventData::provider() const noexcept { return provider_; }

const std::string& EventData::computer() const noexcept { return computer_; }

const std::string& EventData::channel() const noexcept { return channel_; }

const std::string& EventData::description() const noexcept {
  return description_;
}

const std::string& EventData::xml() const noexcept { return xml_; }

const std::string& EventData::user_sid() const noexcept { return user_sid_; }

const std::vector<uint8_t>& EventData::binary_data() const noexcept {
  return binary_data_;
}

const std::unordered_map<std::string, std::string>& EventData::data()
    const noexcept {
  return data_;
}

std::optional<std::string_view> EventData::get_data_field(
    const std::string_view key) const {
  const auto it = data_.find(std::string(key));
  return it != data_.end() ? std::make_optional(std::string_view(it->second))
                           : std::nullopt;
}

std::chrono::system_clock::time_point EventData::system_timepoint()
    const noexcept {
  // Константы для преобразования Windows FILETIME
  constexpr uint64_t FILETIME_EPOCH_DIFF =
      116444736000000000ULL;  ///< Разница эпох в 100-нс интервалах
  constexpr uint64_t NS_PER_FILETIME_TICK =
      100;  ///< Наносекунд в одном интервале FILETIME

  if (timestamp_ <= FILETIME_EPOCH_DIFF) {
    return std::chrono::system_clock::time_point{};
  }

  const uint64_t ns_since_epoch =
      (timestamp_ - FILETIME_EPOCH_DIFF) * NS_PER_FILETIME_TICK;
  return std::chrono::system_clock::time_point{
      std::chrono::duration_cast<std::chrono::system_clock::duration>(
          std::chrono::nanoseconds(ns_since_epoch))};
}

bool EventData::is_error() const noexcept { return is_error_level(level_); }

bool EventData::is_warning() const noexcept { return is_warning_level(level_); }

bool EventData::is_info() const noexcept { return is_info_level(level_); }

EventDataBuilder EventData::builder() noexcept { return EventDataBuilder{}; }

EventData::EventData(uint32_t event_id, uint64_t timestamp, EventLevel level,
                     std::string provider, std::string computer,
                     std::string channel, std::string description,
                     std::string xml, std::string user_sid,
                     std::vector<uint8_t> binary_data,
                     std::unordered_map<std::string, std::string> data)
    : event_id_(event_id),
      timestamp_(timestamp),
      level_(level),
      provider_(std::move(provider)),
      computer_(std::move(computer)),
      channel_(std::move(channel)),
      description_(std::move(description)),
      xml_(std::move(xml)),
      user_sid_(std::move(user_sid)),
      binary_data_(std::move(binary_data)),
      data_(std::move(data)) {}

}
