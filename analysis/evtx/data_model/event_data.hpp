#pragma once

#include <cstdint>
#include <string>
#include "ievent_data.hpp"
#include <memory>
#include <unordered_map>

namespace EventLogAnalysis {

/// @brief Конкретная реализация данных события
class EventData : public IEventData {
public:
  EventData() = default;

  // Setters
  void SetEventID(uint32_t id) { event_id_ = id; }
  void SetTimestamp(uint64_t timestamp) { timestamp_ = timestamp; }
  void SetLevel(EventLevel level) { level_ = level; }
  void SetProvider(const std::string& provider) { provider_ = provider; }
  void SetComputer(const std::string& computer) { computer_ = computer; }
  void SetChannel(const std::string& channel) { channel_ = channel; }
  void SetDescription(const std::string& desc) { description_ = desc; }
  void SetXml(const std::string& xml) { xml_ = xml; }

  void AddData(const std::string& name, const std::string& value) {
    data_[name] = value;
  }

  // Getters (реализация интерфейса)
  uint32_t EventID() const override { return event_id_; }
  uint64_t Timestamp() const override { return timestamp_; }
  EventLevel Level() const override { return level_; }
  const std::string& Provider() const override { return provider_; }
  const std::string& Computer() const override { return computer_; }
  const std::string& Channel() const override { return channel_; }
  const std::string& Description() const override { return description_; }
  const std::unordered_map<std::string, std::string>& Data() const override { return data_; }
  const std::string& XmlRepresentation() const override { return xml_; }

private:
  uint32_t event_id_ = 0;
  uint64_t timestamp_ = 0;
  EventLevel level_ = EventLevel::LOG_ALWAYS;
  std::string provider_;
  std::string computer_;
  std::string channel_;
  std::string description_;
  std::string xml_;
  std::unordered_map<std::string, std::string> data_;
};

}
