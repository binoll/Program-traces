#pragma once

#include "../data/analysis_data.hpp"
#include "../../../../parsers/logs/evtx/parser/parser.hpp"
#include "../../os_detection/os_detection.hpp"
#include <memory>
#include <vector>
#include <map>

namespace WindowsDiskAnalysis {

class EventLogAnalyzer {
public:
  EventLogAnalyzer(std::unique_ptr<EventLogAnalysis::IEventLogParser> parser);

  void collect(
      const std::string& disk_root,
      const OSInfo& os_info,
      std::map<std::string, ProcessInfo>& process_data,
      std::vector<NetworkConnection>& network_connections
  );

private:
  std::unique_ptr<EventLogAnalysis::IEventLogParser> parser_;
};

}
