#pragma once

#include "../data/analysis_data.hpp"
#include <map>
#include <string>

namespace WindowsDiskAnalysis {

class FileAnalyzer {
public:
  void collect(std::map<std::string, ProcessInfo>& process_data);
};

}
