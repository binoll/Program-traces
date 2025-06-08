#pragma once

#include "../data/analysis_data.hpp"
#include "../../../../parsers/prefetch/parser/parser.hpp"
#include "../../os_detection/os_detection.hpp"
#include <memory>
#include <map>

namespace WindowsDiskAnalysis {

class PrefetchAnalyzer {
public:
  PrefetchAnalyzer(std::unique_ptr<PrefetchAnalysis::IPrefetchParser> parser);

  std::map<std::string, ProcessInfo> collect(
      const std::string& disk_root,
      const OSInfo& os_info,
      const Config& config
  );

private:
  std::unique_ptr<PrefetchAnalysis::IPrefetchParser> parser_;
};

}
