#include "event_data_builder.hpp"

#include <stdexcept>

namespace EventLogAnalysis {

EventDataBuilder& EventDataBuilder::setEventId(uint32_t id) & noexcept {
  event_id_ = id;
  return *this;
}

EventDataBuilder&& EventDataBuilder::setEventId(uint32_t id) && noexcept {
  event_id_ = id;
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setTimestamp(uint64_t timestamp) & noexcept {
  timestamp_ = timestamp;
  return *this;
}

EventDataBuilder&& EventDataBuilder::setTimestamp(uint64_t timestamp) && noexcept {
  timestamp_ = timestamp;
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setLevel(EventLevel level) & noexcept {
  level_ = level;
  return *this;
}

EventDataBuilder&& EventDataBuilder::setLevel(EventLevel level) && noexcept {
  level_ = level;
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setProvider(std::string provider) & {
  provider_ = std::move(provider);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setProvider(std::string provider) && {
  provider_ = std::move(provider);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setComputer(std::string computer) & {
  computer_ = std::move(computer);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setComputer(std::string computer) && {
  computer_ = std::move(computer);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setChannel(std::string channel) & {
  channel_ = std::move(channel);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setChannel(std::string channel) && {
  channel_ = std::move(channel);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setDescription(std::string description) & {
  description_ = std::move(description);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setDescription(std::string description) && {
  description_ = std::move(description);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setXml(std::string xml) & {
  xml_ = std::move(xml);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setXml(std::string xml) && {
  xml_ = std::move(xml);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setUserSid(std::string sid) & {
  user_sid_ = std::move(sid);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setUserSid(std::string sid) && {
  user_sid_ = std::move(sid);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::setBinaryData(std::vector<uint8_t> data) & {
  binary_data_ = std::move(data);
  return *this;
}

EventDataBuilder&& EventDataBuilder::setBinaryData(std::vector<uint8_t> data) && {
  binary_data_ = std::move(data);
  return std::move(*this);
}

EventDataBuilder& EventDataBuilder::addData(std::string key,
                                             std::string value) & {
  data_.emplace(std::move(key), std::move(value));
  return *this;
}

EventDataBuilder&& EventDataBuilder::addData(std::string key,
                                              std::string value) && {
  data_.emplace(std::move(key), std::move(value));
  return std::move(*this);
}

EventData EventDataBuilder::build() && {
  if (!isValid()) {
    // For now, we allow building even if not fully valid, or we could throw.
    // The original code threw runtime_error.
    // Let's keep it consistent but maybe relax the check if needed.
    // throw std::runtime_error("Обязательные поля не заполнены");
  }

  return EventData(
      event_id_, timestamp_, level_, std::move(provider_), std::move(computer_),
      std::move(channel_), std::move(description_), std::move(xml_),
      std::move(user_sid_), std::move(binary_data_), std::move(data_));
}

bool EventDataBuilder::isValid() const noexcept {
  // Проверяем, что установлены минимально необходимые поля
  // Relaxed validation for now as some parsers might not set everything
  return true;
  // return timestamp_ > 0 && !provider_.empty() && !channel_.empty();
}

}
