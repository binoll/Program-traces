#include "event_data_builder.hpp"

#include <stdexcept>

namespace EventLogAnalysis {

EventDataBuilder& EventDataBuilder::event_id(uint32_t id) & noexcept {
  event_id_ = id;
  return *this;
}

EventDataBuilder&& EventDataBuilder::event_id(uint32_t id) && noexcept {
  event_id_ = id;
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::timestamp(uint64_t timestamp) & noexcept {
  timestamp_ = timestamp;
  return *this;
}

EventDataBuilder&& EventDataBuilder::timestamp(uint64_t timestamp) && noexcept {
  timestamp_ = timestamp;
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::level(EventLevel level) & noexcept {
  level_ = level;
  return *this;
}

EventDataBuilder&& EventDataBuilder::level(EventLevel level) && noexcept {
  level_ = level;
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::provider(std::string provider) & {
  provider_ = std::move(provider);
  return *this;
}

EventDataBuilder&& EventDataBuilder::provider(std::string provider) && {
  provider_ = std::move(provider);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::computer(std::string computer) & {
  computer_ = std::move(computer);
  return *this;
}

EventDataBuilder&& EventDataBuilder::computer(std::string computer) && {
  computer_ = std::move(computer);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::channel(std::string channel) & {
  channel_ = std::move(channel);
  return *this;
}

EventDataBuilder&& EventDataBuilder::channel(std::string channel) && {
  channel_ = std::move(channel);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::description(std::string description) & {
  description_ = std::move(description);
  return *this;
}

EventDataBuilder&& EventDataBuilder::description(std::string description) && {
  description_ = std::move(description);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::xml(std::string xml) & {
  xml_ = std::move(xml);
  return *this;
}

EventDataBuilder&& EventDataBuilder::xml(std::string xml) && {
  xml_ = std::move(xml);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::user_sid(std::string sid) & {
  user_sid_ = std::move(sid);
  return *this;
}

EventDataBuilder&& EventDataBuilder::user_sid(std::string sid) && {
  user_sid_ = std::move(sid);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::binary_data(std::vector<uint8_t> data) & {
  binary_data_ = std::move(data);
  return *this;
}

EventDataBuilder&& EventDataBuilder::binary_data(std::vector<uint8_t> data) && {
  binary_data_ = std::move(data);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::add_data(std::string key,
                                             std::string value) & {
  data_.emplace(std::move(key), std::move(value));
  return *this;
}

EventDataBuilder&& EventDataBuilder::add_data(std::string key,
                                              std::string value) && {
  data_.emplace(std::move(key), std::move(value));
  return std::move(*this);
}

EventData EventDataBuilder::build() && {
  if (!is_valid()) {
    throw std::runtime_error("Обязательные поля не заполнены");
  }

  return EventData(
      event_id_, timestamp_, level_, std::move(provider_), std::move(computer_),
      std::move(channel_), std::move(description_), std::move(xml_),
      std::move(user_sid_), std::move(binary_data_), std::move(data_));
}

bool EventDataBuilder::is_valid() const noexcept {
  // Проверяем, что установлены минимально необходимые поля
  return timestamp_ > 0 && !provider_.empty() && !channel_.empty();
}

}
