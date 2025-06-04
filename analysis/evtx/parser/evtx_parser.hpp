#pragma once

#include <libevtx.h>

#include <memory>
#include <string>
#include <vector>

#include "../data_model/event_data.hpp"
#include "ievent_log_parser.hpp"

namespace EventLogAnalysis {

class EvtxParser final : public IEventLogParser {
public:
  EvtxParser();
  ~EvtxParser() override;

  EvtxParser(const EvtxParser&) = delete;
  EvtxParser& operator=(const EvtxParser&) = delete;

  void OpenLogFile(const std::string& file_path) override;
  void CloseLogFile() override;

  std::vector<std::unique_ptr<IEventData>> ParseEvents() override;
  std::vector<std::unique_ptr<IEventData>> GetEventsByType(
      uint32_t event_id) override;

private:
  std::unique_ptr<EventData> ParseRecord(libevtx_record_t* record);
  static void ExtractEventDataFromXml(EventData& event_data, const std::string& xml);

  libevtx_file_t* evtx_file_ = nullptr;
  bool file_opened_ = false;
};

}
