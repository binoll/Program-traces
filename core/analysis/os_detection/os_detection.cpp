#include "os_detection.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "../../core/exceptions/registry_exception.hpp"
#include "os_info.hpp"

namespace WindowsVersion {

namespace {

// Вспомогательные функции для работы со строками
std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(str);
  while (std::getline(token_stream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

void trim(std::string& str) {
  // Удаление пробелов в начале
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
              return !std::isspace(ch);
            }));

  // Удаление пробелов в конце
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            str.end());
}

// Разделение строки на две части по разделителю
std::pair<std::string, std::string> splitString(const std::string& input,
                                                char delimiter) {
  const size_t pos = input.find(delimiter);
  if (pos == std::string::npos) {
    return {input, ""};
  }
  return {input.substr(0, pos), input.substr(pos + 1)};
}

}  // namespace

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

  // Загрузка конфигурации для каждой версии
  for (auto& name : version_names) {
    trim(name);
    VersionConfig cfg;

    // Загрузка путей реестра
    const std::string path_config =
        config_.getString("RegistryPaths", name, "");
    if (path_config.empty()) {
      GlobalLogger::get()->warn("Не указаны пути реестра для версии: {}", name);
      continue;
    }

    auto [file_path, key_path] = splitString(path_config, ':');
    trim(file_path);
    trim(key_path);

    if (file_path.empty() || key_path.empty()) {
      GlobalLogger::get()->warn("Некорректные пути реестра для {}: {}", name,
                                path_config);
      continue;
    }

    cfg.registry_file = file_path;
    cfg.registry_key = key_path;

    // Загрузка ключей реестра
    const std::vector<std::string> keys = {
        "ProductName", "CurrentVersion", "CurrentBuild",   "CurrentBuildNumber",
        "EditionID",   "ReleaseId",      "DisplayVersion", "CSDVersion"};

    for (const auto& key : keys) {
      const std::string config_key = name + "::" + key;
      const std::string value_name =
          config_.getString("RegistryKeys", config_key, "");
      if (!value_name.empty()) {
        cfg.registry_values[key] = value_name;
      }
    }

    if (cfg.registry_values.empty()) {
      GlobalLogger::get()->warn("Не указаны ключи реестра для версии: {}",
                                name);
      continue;
    }

    version_configs_[name] = std::move(cfg);
  }

  if (version_configs_.empty()) {
    throw OSDetectionException(
        "Нет корректных конфигураций для определения ОС");
  }
}

OSInfo OSDetection::detect() const {
  const auto logger = GlobalLogger::get();
  OSInfo info;
  bool detected = false;

  logger->info("Начало определения версии Windows...");

  for (const auto& [version_name, cfg] : version_configs_) {
    try {
      // Формируем полный путь к файлу реестра
      const std::string full_path = device_root_path_ + cfg.registry_file;

      logger->debug("Проверка {}: файл={}, ключ={}", version_name, full_path,
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

  determineRealOSName(info);

  logger->info("Операционная система: {}. Сборка: {}", info.full_os_name,
               info.build_number);
  return info;
}

std::string OSDetection::getLastComponent(const std::string& path) {
  const size_t pos = path.find_last_of("/\\");
  if (pos != std::string::npos && pos < path.size() - 1) {
    return path.substr(pos + 1);
  }
  return path;
}

void OSDetection::extractOSInfo(
    const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>& values,
    OSInfo& info, const std::string& version_name) const {
  const auto logger = GlobalLogger::get();
  const auto& cfg = version_configs_.at(version_name);

  // Функция для получения значения реестра
  auto getValue = [&](const std::string& key_name, std::string& target) {
    auto it = cfg.registry_values.find(key_name);
    if (it == cfg.registry_values.end()) {
      logger->debug("Ключ не настроен: {}::{}", version_name, key_name);
      return;
    }

    const std::string& search_key = getLastComponent(it->second);
    target.clear();

    for (const auto& data : values) {
      const std::string data_key = getLastComponent(data->getName());

      if (data_key == search_key) {
        try {
          if (data->getType() == RegistryAnalysis::RegistryValueType::REG_SZ ||
              data->getType() ==
                  RegistryAnalysis::RegistryValueType::REG_EXPAND_SZ) {
            target = data->getAsString();
            logger->debug("Найден параметр реестра: {} = {}", search_key,
                          target);
            return;
          }
        } catch (...) {
          logger->warn("Ошибка чтения параметра: {}", search_key);
        }
      }
    }
    logger->debug("Параметр не найден: {}", search_key);
  };

  // Основная информация
  getValue("ProductName", info.product_name);
  getValue("CurrentVersion", info.version);

  // Информация о сборке
  getValue("CurrentBuild", info.build_number);
  if (info.build_number.empty()) {
    getValue("CurrentBuildNumber", info.build_number);
  }

  // Дополнительная информация
  getValue("EditionID", info.edition);
  getValue("ReleaseId", info.release);
  getValue("DisplayVersion", info.display_version);

  // Для старых версий Windows (XP)
  if (info.release.empty()) {
    getValue("CSDVersion", info.release);
  }

  // Проверка обязательных полей
  if (info.product_name.empty() || info.version.empty() ||
      info.build_number.empty()) {
    throw OSDetectionException("Недостаточно данных для определения версии ОС");
  }
}

void OSDetection::determineRealOSName(OSInfo& info) {
  const bool is_server = isServerSystem(info);

  try {
    const int build_number = std::stoi(info.build_number);
    info.build_version = resolveBuildVersion(build_number, is_server);
  } catch (...) {
    info.build_version = WindowsBuild::UNKNOWN;
  }

  info.full_os_name = getOSName(info);
}

bool OSDetection::isServerSystem(const OSInfo& info) {
  const std::string& product = info.product_name;
  const std::string& edition = info.edition;

  // Ключевые слова для серверных версий
  const std::vector<std::string> keywords = {
      "Server",     "Datacenter", "Enterprise",     "Storage",    "Cluster",
      "Foundation", "Essentials", "Small Business", "MultiPoint", "Hyper-V",
      "Azure",      "Cloud",      "Nano",           "Web"};

  auto containsKeyword = [&](const std::string& str) {
    for (const auto& keyword : keywords) {
      if (str.find(keyword) != std::string::npos) {
        return true;
      }
    }
    return false;
  };

  return containsKeyword(product) || containsKeyword(edition);
}

WindowsBuild OSDetection::resolveBuildVersion(int build_number,
                                              bool is_server) {
  // Клиентские версии Windows
  static const std::map<int, WindowsBuild> client_builds = {
      {2600, WindowsBuild::XP},          {3790, WindowsBuild::XP_x64},
      {6000, WindowsBuild::VISTA},       {6001, WindowsBuild::VISTA_SP1},
      {6002, WindowsBuild::VISTA_SP2},   {7600, WindowsBuild::WIN7},
      {7601, WindowsBuild::WIN7_SP1},    {9200, WindowsBuild::WIN8},
      {9600, WindowsBuild::WIN8_1},      {10240, WindowsBuild::WIN10_1507},
      {10586, WindowsBuild::WIN10_1511}, {14393, WindowsBuild::WIN10_1607},
      {15063, WindowsBuild::WIN10_1703}, {16299, WindowsBuild::WIN10_1709},
      {17134, WindowsBuild::WIN10_1803}, {17763, WindowsBuild::WIN10_1809},
      {18362, WindowsBuild::WIN10_1903}, {18363, WindowsBuild::WIN10_1909},
      {19041, WindowsBuild::WIN10_2004}, {19042, WindowsBuild::WIN10_20H2},
      {19043, WindowsBuild::WIN10_21H1}, {19044, WindowsBuild::WIN10_21H2},
      {19045, WindowsBuild::WIN10_22H2}, {22000, WindowsBuild::WIN11_21H2},
      {22621, WindowsBuild::WIN11_22H2}, {22631, WindowsBuild::WIN11_23H2},
      {26100, WindowsBuild::WIN11_24H2}};

  // Серверные версии Windows
  static const std::map<int, WindowsBuild> server_builds = {
      {3790, WindowsBuild::SERVER_2003},
      {6001, WindowsBuild::SERVER_2008},
      {6002, WindowsBuild::SERVER_2008_SP2},
      {7600, WindowsBuild::SERVER_2008_R2},
      {7601, WindowsBuild::SERVER_2008_R2_SP1},
      {9200, WindowsBuild::SERVER_2012},
      {9600, WindowsBuild::SERVER_2012_R2},
      {14393, WindowsBuild::SERVER_2016},
      {17763, WindowsBuild::SERVER_2019},
      {20348, WindowsBuild::SERVER_2022},
      {20350, WindowsBuild::SERVER_2025}};

  const auto& build_map = is_server ? server_builds : client_builds;
  auto it = build_map.find(build_number);

  return (it != build_map.end()) ? it->second : WindowsBuild::UNKNOWN;
}

std::string OSDetection::getOSName(const OSInfo& info) {
  const bool is_server = isServerSystem(info);
  std::string name = info.product_name;  // Значение по умолчанию

  if (info.build_version != WindowsBuild::UNKNOWN) {
    if (is_server) {
      switch (info.build_version) {
        case WindowsBuild::SERVER_2003:
          name = "Windows Server 2003";
          break;
        case WindowsBuild::SERVER_2008:
          name = "Windows Server 2008";
          break;
        case WindowsBuild::SERVER_2008_SP2:
          name = "Windows Server 2008 SP2";
          break;
        case WindowsBuild::SERVER_2008_R2:
          name = "Windows Server 2008 R2";
          break;
        case WindowsBuild::SERVER_2008_R2_SP1:
          name = "Windows Server 2008 R2 SP1";
          break;
        case WindowsBuild::SERVER_2012:
          name = "Windows Server 2012";
          break;
        case WindowsBuild::SERVER_2012_R2:
          name = "Windows Server 2012 R2";
          break;
        case WindowsBuild::SERVER_2016:
          name = "Windows Server 2016";
          break;
        case WindowsBuild::SERVER_2019:
          name = "Windows Server 2019";
          break;
        case WindowsBuild::SERVER_2022:
          name = "Windows Server 2022";
          break;
        case WindowsBuild::SERVER_2025:
          name = "Windows Server 2025";
          break;
        default:
          break;
      }
    } else {
      switch (info.build_version) {
        case WindowsBuild::XP:
          name = "Windows XP";
          break;
        case WindowsBuild::XP_x64:
          name = "Windows XP x64 Edition";
          break;
        case WindowsBuild::VISTA:
          name = "Windows Vista";
          break;
        case WindowsBuild::VISTA_SP1:
          name = "Windows Vista SP1";
          break;
        case WindowsBuild::VISTA_SP2:
          name = "Windows Vista SP2";
          break;
        case WindowsBuild::WIN7:
          name = "Windows 7";
          break;
        case WindowsBuild::WIN7_SP1:
          name = "Windows 7 SP1";
          break;
        case WindowsBuild::WIN8:
          name = "Windows 8";
          break;
        case WindowsBuild::WIN8_1:
          name = "Windows 8.1";
          break;
        case WindowsBuild::WIN10_1507:
          name = "Windows 10 1507";
          break;
        case WindowsBuild::WIN10_1511:
          name = "Windows 10 1511";
          break;
        case WindowsBuild::WIN10_1607:
          name = "Windows 10 1607";
          break;
        case WindowsBuild::WIN10_1703:
          name = "Windows 10 1703";
          break;
        case WindowsBuild::WIN10_1709:
          name = "Windows 10 1709";
          break;
        case WindowsBuild::WIN10_1803:
          name = "Windows 10 1803";
          break;
        case WindowsBuild::WIN10_1809:
          name = "Windows 10 1809";
          break;
        case WindowsBuild::WIN10_1903:
          name = "Windows 10 1903";
          break;
        case WindowsBuild::WIN10_1909:
          name = "Windows 10 1909";
          break;
        case WindowsBuild::WIN10_2004:
          name = "Windows 10 2004";
          break;
        case WindowsBuild::WIN10_20H2:
          name = "Windows 10 20H2";
          break;
        case WindowsBuild::WIN10_21H1:
          name = "Windows 10 21H1";
          break;
        case WindowsBuild::WIN10_21H2:
          name = "Windows 10 21H2";
          break;
        case WindowsBuild::WIN10_22H2:
          name = "Windows 10 22H2";
          break;
        case WindowsBuild::WIN11_21H2:
          name = "Windows 11 21H2";
          break;
        case WindowsBuild::WIN11_22H2:
          name = "Windows 11 22H2";
          break;
        case WindowsBuild::WIN11_23H2:
          name = "Windows 11 23H2";
          break;
        case WindowsBuild::WIN11_24H2:
          name = "Windows 11 24H2";
          break;
        default:
          break;
      }
    }
  }

  // Добавление информации о сервис-паках
  if (!info.release.empty() &&
      info.release.find("Service Pack") != std::string::npos) {
    name += " " + info.release;
  }

  // Добавление информации о редакции
  if (!info.edition.empty()) {
    name += " " + info.edition;
  }

  // Уточнение для серверных версий
  if (is_server && name.find("Server") == std::string::npos) {
    name += " Server";
  }

  return name;
}

}
