#include "autorun_analyzer.hpp"

#include <filesystem>
#include <sstream>

#include "../../../../utils/logging/logger.hpp"

// Функция для удаления пробелов в начале и конце строки
void trim(std::string& str) {
  const auto start = std::ranges::find_if_not(
      str, [](unsigned char c) { return std::isspace(c); });
  const auto end =
      std::find_if_not(str.rbegin(), str.rend(), [](const unsigned char c) {
        return std::isspace(c);
      }).base();
  str = (start < end) ? std::string(start, end) : std::string();
}

std::vector<std::string> split(const std::string& str, const char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(str);

  while (std::getline(token_stream, token, delimiter)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }

  return tokens;
}

namespace WindowsDiskAnalysis {

AutorunAnalyzer::AutorunAnalyzer(
    std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
    const std::string& ini_path)
    : parser_(std::move(parser)) {
  const auto logger = GlobalLogger::get();

  // Загрузка конфигурации из INI-файла
  const Config config(ini_path, false, false);

  if (config.hasSection("Autorun") && config.hasKey("Autorun", "Keys")) {
    const std::string keys_str = config.getString("Autorun", "Keys", "");

    for (auto keys = split(keys_str, ','); auto& key : keys) {
      trim(key);
      // Заменяем обратные слеши на прямые
      std::ranges::replace(key, '\\', '/');
      autorun_locations_.push_back(key);
    }
  }
}

std::vector<AutorunEntry> AutorunAnalyzer::collect(const std::string& disk_root,
                                                   WindowsBuild build) {
  std::vector<AutorunEntry> results;
  const auto logger = GlobalLogger::get();

  for (const auto& location : autorun_locations_) {
    const size_t pos = location.find('/');
    if (pos == std::string::npos) continue;

    std::string hive_file = location.substr(0, pos);
    std::string reg_path = disk_root + "/Windows/system32/config/" + hive_file;

    if (!std::filesystem::exists(reg_path)) continue;

    try {
      for (auto values = parser_->getKeyValues(reg_path, location);
           const auto& value : values) {
        results.push_back(
            {value->getName(), value->getDataAsString(), location});
      }
    } catch (...) {
      logger->warn("Ключ пропущен: {}", location);
    }
  }

  return results;
}

}
