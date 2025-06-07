#include "os_detection.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "../../../core/exceptions/os_detection_exception.hpp"
#include "../../../core/exceptions/registry_exception.hpp"
#include "os_info.hpp"

std::vector<std::string> split(const std::string& str, const char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(str);
  while (std::getline(token_stream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

void trim(std::string& str) {
  str.erase(str.begin(), std::ranges::find_if(str, [](const int ch) {
              return !std::isspace(ch);
            }));
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         [](const int ch) { return !std::isspace(ch); })
                .base(),
            str.end());
}

std::string getLastPathComponent(const std::string& path,
                                 const char separator) {
  if (path.empty()) return path;

  size_t end = path.length();
  // Пропускаем завершающие разделители
  while (end > 0 && path[end - 1] == separator) {
    --end;
  }
  if (end == 0) return "";

  // Находим начало последнего компонента
  size_t start = path.find_last_of(separator, end - 1);
  if (start == std::string::npos) {
    return path.substr(0, end);
  }
  return path.substr(start + 1, end - start - 1);
}

namespace WindowsVersion {

OSDetection::OSDetection(
    std::unique_ptr<RegistryAnalysis::IRegistryParser> parser, Config&& config,
    std::string device_root_path)
    : parser_(std::move(parser)),
      config_(std::move(config)),
      device_root_path_(std::move(device_root_path)) {
  loadConfiguration();
}

void OSDetection::loadConfiguration() {
  // Загрузка списка версий
  const std::string version_list = config_.getString("General", "Versions", "");
  if (version_list.empty()) {
    throw OSDetectionException(
        "Ошибка конфигурации: Отсутствует параметр 'Versions' в секции "
        "[General]");
  }

  auto version_names = split(version_list, ',');
  const auto logger = GlobalLogger::get();

  // Загрузка конфигурации для каждой версии
  for (auto& name : version_names) {
    trim(name);
    VersionConfig cfg;

    // Загрузка пути к файлу реестра
    cfg.registry_file = config_.getString("RegistryPaths", name, "");
    if (cfg.registry_file.empty()) {
      logger->warn("Не указан путь к файлу реестра для версии: {}", name);
      continue;
    }

    // Загрузка пути к ключу реестра
    cfg.registry_key = config_.getString("OSInfoHive", name, "");
    if (cfg.registry_key.empty()) {
      logger->warn("Не указан путь к ключу реестра для версии: {}", name);
      continue;
    }

    // Загрузка ключей реестра из конфига
    const std::string config_keys = config_.getString("OSInfoKeys", name, "");
    if (config_keys.empty()) {
      logger->warn("Не указаны ключи реестра для версии: {}", name);
      continue;
    }

    // Парсинг ключей
    for (auto keys = split(config_keys, ','); auto& key : keys) {
      trim(key);
      if (!key.empty()) {
        cfg.registry_keys.push_back(key);
      }
    }

    if (cfg.registry_keys.empty()) {
      logger->warn("Список ключей реестра пуст для версии: {}", name);
      continue;
    }

    version_configs_[name] = std::move(cfg);
    logger->debug("Загружена конфигурация для {}: {} ключей", name,
                  cfg.registry_keys.size());
  }

  // Загрузка ключевых слов серверных ОС
  const std::string keywords_str =
      config_.getString("OSKeywords", "DefaultServerKeywords", "");
  if (!keywords_str.empty()) {
    auto keywords = split(keywords_str, ',');
    for (auto& kw : keywords) {
      trim(kw);
      if (!kw.empty()) {
        default_server_keywords_.push_back(kw);
      }
    }
    logger->debug("Загружено ключевых слов серверных ОС: {}",
                  default_server_keywords_.size());
  } else {
    logger->debug("Для DefaultServerKeywords не указаны ключевые слова");
  }

  const std::string client_section = "BuildMappingsClient";
  std::vector<std::string> client_keys;
  try {
    client_keys = config_.getKeysInSection(client_section);
  } catch (const std::exception& e) {
    logger->warn("Ошибка при получении ключей из секции {}: {}", client_section,
                 e.what());
  }

  for (const auto& build_str : client_keys) {
    try {
      uint32_t build_num = std::stoul(build_str);
      const std::string os_name =
          config_.getString(client_section, build_str, "");
      if (!os_name.empty()) {
        client_builds[build_num] = os_name;
      }
    } catch (const std::exception& e) {
      logger->warn("Ошибка обработки сборки {}: {}", build_str, e.what());
    }
  }
  logger->debug("Загружено клиентских сборок: {}", client_builds.size());

  // ЗАГРУЗКА СЕРВЕРНЫХ СБОРОК
  const std::string server_section = "BuildMappingsServer";
  std::vector<std::string> server_keys;
  try {
    server_keys = config_.getKeysInSection(server_section);
  } catch (const std::exception& e) {
    logger->warn("Ошибка при получении ключей из секции {}: {}", server_section,
                 e.what());
  }

  for (const auto& build_str : server_keys) {
    try {
      uint32_t build_num = std::stoul(build_str);
      const std::string os_name =
          config_.getString(server_section, build_str, "");
      if (!os_name.empty()) {
        server_builds[build_num] = os_name;
      }
    } catch (const std::exception& e) {
      logger->warn("Ошибка обработки сборки {}: {}", build_str, e.what());
    }
  }
  logger->debug("Загружено серверных сборок: {}", server_builds.size());

  if (version_configs_.empty()) {
    throw OSDetectionException(
        "Нет корректных конфигураций для определения ОС");
  }
}

OSInfo OSDetection::detect() {
  const auto logger = GlobalLogger::get();
  OSInfo info;
  bool detected = false;

  logger->info("Начало определения версии Windows...");

  for (const auto& [version_name, cfg] : version_configs_) {
    try {
      const std::string full_path = device_root_path_ + cfg.registry_file;

      logger->debug("Проверка {}. Файл={}. Ключ={}", version_name, full_path,
                    cfg.registry_key);

      const auto values = parser_->getKeyValues(full_path, cfg.registry_key);
      if (values.empty()) {
        logger->debug("Ключ реестра не найден для {}", version_name);
        continue;
      }

      extractOSInfo(values, info, version_name);
      detected = true;
      break;
    } catch (const std::exception& e) {
      logger->debug("Ошибка при проверке {}: {}", version_name, e.what());
    }
  }

  if (!detected) {
    const std::string error = "Не удалось определить версию ОС";
    logger->error(error);
    throw OSDetectionException(error);
  }

  determineFullOSName(info);

  logger->info("Операционная система: {}. Сборка: {}", info.full_os_name,
               info.current_build);
  return info;
}

void OSDetection::extractOSInfo(
    const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>& values,
    OSInfo& info, const std::string& version_name) const {
  const auto logger = GlobalLogger::get();

  // Проверяем наличие конфигурации для данной версии
  if (!version_configs_.contains(version_name)) {
    throw OSDetectionException("конфигурация для версии " + version_name +
                               " не найдена");
  }

  const auto& cfg = version_configs_.at(version_name);
  bool has_essential_data = false;

  // Функция для поиска значения в реестре
  auto findRegistryValue = [&](const std::string& key_name,
                               std::string& target) {
    for (const auto& data : values) {
      if (getLastPathComponent(data->getName()) == key_name) {
        try {
          if (data->getType() == RegistryAnalysis::RegistryValueType::REG_SZ ||
              data->getType() ==
                  RegistryAnalysis::RegistryValueType::REG_EXPAND_SZ) {
            target = data->getAsString();
            logger->debug("Найдено значение реестра: {} = {}", key_name,
                          target);
            return true;
          }
        } catch (...) {
          logger->warn("Ошибка чтения параметра: {}", key_name);
        }
      }
    }
    logger->debug("Параметр не найден: {}", key_name);
    return false;
  };

  // Обрабатываем все ключи для данной версии ОС
  for (const auto& key : cfg.registry_keys) {
    if (std::string value; findRegistryValue(key, value)) {
      if (key == "ProductName") {
        info.product_name = value;
        has_essential_data = true;
      } else if (key == "CurrentVersion") {
        info.current_version = value;
        has_essential_data = true;
      } else if (key == "CurrentBuild" || key == "CurrentBuildNumber") {
        info.current_build = value;
        has_essential_data = true;
      } else if (key == "EditionID") {
        info.edition_id = value;
      } else if (key == "ReleaseId") {
        info.release_id = value;
      } else if (key == "DisplayVersion") {
        info.display_version = value;
      } else if (key == "CSDVersion") {
        info.release_id = value;  // Для обратной совместимости с XP
      }
    }
  }

  // Проверка обязательных полей
  if (!has_essential_data || info.product_name.empty() ||
      (info.current_version.empty() && info.current_build.empty())) {
    throw OSDetectionException("Недостаточно данных для определения версии ОС");
  }
}

void OSDetection::determineFullOSName(OSInfo& info) const {
  const bool is_server = isServerSystem(info);
  std::string name;

  // Пытаемся определить имя ОС по номеру сборки
  try {
    const uint32_t build = std::stoul(info.current_build);
    const auto& build_map = is_server ? server_builds : client_builds;
    const auto it = build_map.find(build);

    if (it != build_map.end()) {
      // Если номер сборки найден в карте, используем соответствующее имя
      name = it->second;
    } else {
      // Если не найден, используем стандартное имя продукта
      name = info.product_name;
    }
  } catch (...) {
    // При ошибке преобразования используем стандартное имя продукта
    name = info.product_name;
  }

  if (!info.edition_id.empty()) {
    name += " " + info.edition_id;
  }

  if (!info.display_version.empty()) {
    name += " " + info.display_version;
  }

  if (!info.release_id.empty()) {
    name += " " + info.release_id;
  }

  if (!info.release_id.empty()) {
    name += " " + info.current_build;
  }

  info.full_os_name = name;
}

bool OSDetection::isServerSystem(const OSInfo& info) const {
  const std::string& product = info.product_name;
  const std::string& edition = info.edition_id;

  auto containsKeyword = [&](const std::string& str) {
    for (const auto& keyword : default_server_keywords_) {
      if (str.find(keyword) != std::string::npos) {
        return true;
      }
    }
    return false;
  };

  return containsKeyword(product) || containsKeyword(edition);
}

}
