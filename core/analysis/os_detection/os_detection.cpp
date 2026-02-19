#include "os_detection.hpp"

#include <algorithm>
#include <filesystem>
#include <utility>

#include "../../../utils/config/config.hpp"
#include "../../../utils/logging/logger.hpp"
#include "../../../utils/utils.hpp"

namespace fs = std::filesystem;

namespace WindowsVersion {

OSDetection::OSDetection(
    std::unique_ptr<RegistryAnalysis::IRegistryParser> parser, Config&& config,
    std::string device_root_path)
    : parser_(std::move(parser)),
      config_(std::move(config)),
      device_root_path_(std::move(device_root_path)) {
  loadConfiguration();
}

OSInfo OSDetection::detect() {
  const auto logger = GlobalLogger::get();
  OSInfo info;

  for (const auto& [version_name, config] : version_configs_) {
    try {
      std::string full_path = device_root_path_ + config.registry_file;

      if (!fs::exists(full_path)) {
        logger->debug("Файл реестра не найден: \"{}\"", full_path);
        continue;
      }

      auto values = parser_->getKeyValues(full_path, config.registry_key);
      if (values.empty()) {
        continue;
      }

      extractOSInfo(values, info, version_name);
      determineFullOSName(info);

      if (!info.product_name.empty()) {
        logger->info("Определена ОС: \"{}\"", info.fullname_os);
        return info;
      }
    } catch (const std::exception& e) {
      logger->warn("Ошибка при проверке версии \"{}\": \"{}\"", version_name,
                   e.what());
    }
  }

  throw OSDetectionException("Не удалось определить версию ОС");
}

void OSDetection::loadConfiguration() {
  const auto logger = GlobalLogger::get();
  std::string versions_str = config_.getString("General", "Versions", "");
  auto versions = split(versions_str, ',');

  for (auto& version : versions) {
    trim(version);
    if (version.empty()) continue;

    VersionConfig cfg;
    cfg.registry_file = config_.getString(version, "RegistryFile", "");
    cfg.registry_key = config_.getString(version, "RegistryKey", "");

    std::string keys_str = config_.getString(version, "RegistryValueNames", "");
    auto keys = split(keys_str, ',');
    for (auto& key : keys) {
      trim(key);
      if (!key.empty()) {
        cfg.registry_keys.push_back(key);
      }
    }

    version_configs_[version] = cfg;
  }

  // Загрузка ключевых слов для серверных ОС
  std::string server_keywords =
      config_.getString("General", "ServerKeywords", "");
  auto keywords = split(server_keywords, ',');
  for (auto& kw : keywords) {
    trim(kw);
    if (!kw.empty()) {
      default_server_keywords_.push_back(kw);
    }
  }

  // Загрузка маппинга сборок
  auto loadBuilds = [&](const std::string& section,
                        std::map<uint32_t, std::string>& target_map) {
    auto values = config_.getAllValues(section);
    for (const auto& [build_str, name] : values) {
      try {
        target_map[std::stoul(build_str)] = name;
      } catch (...) {
        logger->warn("Некорректный номер сборки в конфигурации: \"{}\"",
                     build_str);
      }
    }
  };

  loadBuilds("WindowsClientBuilds", client_builds);
  loadBuilds("WindowsServerBuilds", server_builds);
}

void OSDetection::extractOSInfo(
    const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>& values,
    OSInfo& info, const std::string& version_name) const {
  for (const auto& value : values) {
    std::string name = value->getName();
    std::string data = value->getDataAsString();

    if (name == "ProductName") info.product_name = data;
    else if (name == "CurrentVersion") info.current_version = data;
    else if (name == "EditionID") info.edition_id = data;
    else if (name == "CurrentBuild") info.current_build = data;
    else if (name == "ReleaseId") info.release_id = data;
    else if (name == "DisplayVersion") info.display_version = data;
  }
  info.ini_version = version_name;
}

void OSDetection::determineFullOSName(OSInfo& info) const {
  uint32_t build_num = 0;
  try {
    if (!info.current_build.empty()) {
      build_num = std::stoul(info.current_build);
    }
  } catch (...) {
    // Игнорируем ошибки парсинга номера сборки
  }

  std::string os_name;
  bool is_server = isServerSystem(info);

  // Попытка определить по номеру сборки
  if (build_num > 0) {
    const auto& builds_map = is_server ? server_builds : client_builds;
    auto it = builds_map.find(build_num);
    if (it != builds_map.end()) {
      os_name = it->second;
    } else {
      // Поиск ближайшей меньшей сборки
      auto lb = builds_map.lower_bound(build_num);
      if (lb != builds_map.begin()) {
        os_name = std::prev(lb)->second;
      }
    }
  }

  // Если не удалось определить по сборке, используем ProductName
  if (os_name.empty()) {
    os_name = info.product_name;
  }

  // Формирование полного имени
  info.fullname_os = os_name;
  if (!info.edition_id.empty()) {
    info.fullname_os += " " + info.edition_id;
  }
  if (!info.display_version.empty()) {
    info.fullname_os += " " + info.display_version;
  }
  if (!info.current_build.empty()) {
    info.fullname_os += " (Build " + info.current_build + ")";
  }
}

bool OSDetection::isServerSystem(const OSInfo& info) const {
  std::string product_name_lower = info.product_name;
  std::transform(product_name_lower.begin(), product_name_lower.end(),
                 product_name_lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  for (const auto& keyword : default_server_keywords_) {
    std::string kw_lower = keyword;
    std::transform(kw_lower.begin(), kw_lower.end(), kw_lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (product_name_lower.find(kw_lower) != std::string::npos) {
      return true;
    }
  }

  return false;
}

}
