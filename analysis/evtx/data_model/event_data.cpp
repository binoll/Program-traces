#include "event_data.hpp"

namespace EventLogAnalysis {

EventData::EventData()
    : event_id_(0), timestamp_(0), level_(EventLevel::LOG_ALWAYS) {}

void EventData::SetEventID(uint32_t id) { event_id_ = id; }

void EventData::SetTimestamp(uint64_t timestamp) { timestamp_ = timestamp; }

void EventData::SetLevel(EventLevel level) { level_ = level; }

void EventData::SetProvider(const std::string& provider) {
  provider_ = provider;
}

void EventData::SetComputer(const std::string& computer) {
  computer_ = computer;
}

void EventData::SetChannel(const std::string& channel) { channel_ = channel; }

void EventData::SetDescription(const std::string& desc) { description_ = desc; }

void EventData::SetXml(const std::string& xml) { xml_ = xml; }

void EventData::AddData(const std::string& name, const std::string& value) {
  data_[name] = value;
}

uint32_t EventData::EventID() const { return event_id_; }

uint64_t EventData::Timestamp() const { return timestamp_; }

EventLevel EventData::Level() const { return level_; }

const std::string& EventData::Provider() const { return provider_; }

const std::string& EventData::Computer() const { return computer_; }

const std::string& EventData::Channel() const { return channel_; }

const std::string& EventData::Description() const { return description_; }

const std::unordered_map<std::string, std::string>& EventData::Data() const {
  return data_;
}

const std::string& EventData::XmlRepresentation() const { return xml_; }

}
