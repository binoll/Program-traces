#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

namespace WindowsDiskAnalysis {

struct AutorunEntry {
  std::string name;
  std::string path;
  std::string command;
  std::string location;
};

struct ProcessInfo {
  std::string executable_path;
  std::vector<uint64_t> run_times;
  uint32_t run_count = 0;
  std::string hash;
  bool deleted = false;
  time_t creation_time = 0;
  time_t modification_time = 0;
};

struct NetworkConnection {
  std::string process_name;
  std::string local_address;
  std::string remote_address;
  uint16_t port = 0;
  std::string protocol;
};

}
