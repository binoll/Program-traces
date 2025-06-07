#pragma once

#include <algorithm>
#include <cctype>
#include <memory>
#include <vector>

#include "../../../../parsers/registry/parser/parser.hpp"
#include "../../../../utils/config/config.hpp"
#include "../../os_detection/ios_detection.hpp"
#include "../data/analysis_data.hpp"

void trim(std::string& str);

std::vector<std::string> split(const std::string& str, char delimiter);

namespace WindowsDiskAnalysis {

class AutorunAnalyzer {
public:
  // Добавляем путь к INI-файлу в параметры конструктора
  AutorunAnalyzer(
    std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
    const std::string& ini_path
  );

  std::vector<AutorunEntry> collect(const std::string& disk_root, WindowsBuild build);

private:
  std::unique_ptr<RegistryAnalysis::IRegistryParser> parser_;
  std::vector<std::string> autorun_locations_; // Список путей из INI
};

}
