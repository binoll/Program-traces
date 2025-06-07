#pragma once

#include "analysis/autorun_analyzer.hpp"
#include "analysis/eventlog_analyzer.hpp"
#include "analysis/file_analyzer.hpp"
#include "analysis/prefetch_analyzer.hpp"
#include "../../../utils/export/csv_exporter.hpp"
#include "../os_detection/os_detection.hpp"
#include "../../../utils/config/config.hpp"
#include <memory>

namespace WindowsDiskAnalysis {

class WindowsDiskAnalyzer {
public:
  WindowsDiskAnalyzer(const std::string& disk_root, const std::string& config_path);
  void analyze(const std::string& output_path);

private:
  void initializeComponents();
  void detectOSVersion();

  std::string disk_root_;
  Config config_;
  OSInfo os_info_;

  std::unique_ptr<AutorunAnalyzer> autorun_analyzer_;
  std::unique_ptr<PrefetchAnalyzer> prefetch_analyzer_;
  std::unique_ptr<EventLogAnalyzer> eventlog_analyzer_;
  std::unique_ptr<FileAnalyzer> file_analyzer_;

  std::vector<AutorunEntry> autorun_entries_;
  std::map<std::string, ProcessInfo> process_data_;
  std::vector<NetworkConnection> network_connections_;
};

}
