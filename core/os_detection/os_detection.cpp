#include "os_detection.hpp"

#include <map>

#include "../../core/exceptions/registry_exception.hpp"
#include "os_info.hpp"

namespace WindowsVersion {

OSInfo OSDetection::detect() const {
  const auto logger = GlobalLogger::get();
  const std::string key_path = "Microsoft/Windows NT/CurrentVersion";
  const std::string registry_path =
      software_file_path_ + "/Windows/System32/config/SOFTWARE";
  OSInfo info;

  logger->info("Определение версии ОС по реестру: {}", registry_path);

  try {
    logger->debug("Чтение ключа: {}", key_path);
    const auto values = parser_->getKeyValues(registry_path, key_path);

    if (values.empty()) {
      logger->error("Ключ реестра не найден: {}", key_path);
      throw RegistryKeyNotFoundException(key_path);
    }

    extractOSInfo(values, info);
    determineRealOSName(info);

    logger->info("Определена ОС: {} (сборка {})", info.real_os_name,
                 info.build_number);
    logger->debug("Детали: {}, {}, {}, {}", info.product_name, info.version,
                  info.edition, info.display_version);
  } catch (const std::exception& e) {
    logger->error("Ошибка: {}", e.what());
    throw OSDetectionException("Ошибка определения ОС");
  }

  return info;
}

void OSDetection::extractOSInfo(
    const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>& values,
    OSInfo& info) {
  const auto logger = GlobalLogger::get();

  auto getValue = [&](const std::string& name, std::string& target,
                      bool required = false) {
    target = getStringValue(values, name);

    if (required && target.empty()) {
      logger->error("Обязательный параметр отсутствует: {}", name);
      throw RegistryAnalysis::ValueNotFoundError(name, "");
    }

    if (!target.empty()) {
      logger->debug("Параметр {} = {}", name, target);
    }
  };

  // Основные параметры
  getValue("ProductName", info.product_name, true);
  getValue("CurrentVersion", info.version, true);

  // Параметры сборки
  getValue("CurrentBuild", info.build_number);
  if (info.build_number.empty()) {
    getValue("CurrentBuildNumber", info.build_number, true);
  }

  // Дополнительные параметры
  getValue("EditionID", info.edition);
  getValue("ReleaseId", info.release_id);
  getValue("DisplayVersion", info.display_version);

  // Для старых версий Windows
  if (info.release_id.empty()) {
    getValue("CSDVersion", info.release_id);
  }
}

bool OSDetection::isServerSystem(const OSInfo& info) {
  const std::string& product = info.product_name;
  const std::string& edition = info.edition;

  // Проверка по ключевым словам в названии продукта и редакции
  auto containsServerKeyword = [](const std::string& str) {
    const std::vector<std::string> keywords = {
        "Server",     "Datacenter", "Enterprise",     "Storage",    "Cluster",
        "Foundation", "Essentials", "Small Business", "MultiPoint", "Hyper-V",
        "Azure",      "Cloud",      "Nano",           "Web"};

    for (const auto& keyword : keywords) {
      if (str.find(keyword) != std::string::npos) {
        return true;
      }
    }
    return false;
  };

  return containsServerKeyword(product) || containsServerKeyword(edition);
}

void OSDetection::determineRealOSName(OSInfo& info) {
  // Проверка серверной системы
  const bool isServer = isServerSystem(info);

  // Преобразование номера сборки
  int build_number = 0;
  try {
    build_number = std::stoi(info.build_number);
  } catch (const std::exception&) {
    info.build_version = WindowsBuild::UNKNOWN;
    info.real_os_name = info.product_name;
    return;
  }

  // Определение версии сборки
  info.build_version = resolveBuildVersion(build_number, isServer);
  info.real_os_name = getOSName(info);
}

WindowsBuild OSDetection::resolveBuildVersion(int build_number, bool isServer) {
  // Карты для преобразования номера сборки
  static const std::map<int32_t, WindowsBuild> client_builds = {
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

  // Выбор карты в зависимости от типа системы
  const auto& build_map = isServer ? server_builds : client_builds;
  auto it = build_map.find(build_number);

  if (it != build_map.end()) {
    return it->second;
  }

  // Попытка найти ближайшую известную сборку
  if (!build_map.empty()) {
    auto lower = build_map.lower_bound(build_number);
    if (lower != build_map.begin()) {
      return (--lower)->second;
    }
  }

  return WindowsBuild::UNKNOWN;
}

std::string OSDetection::getOSName(const OSInfo& info) {
  std::string name;
  const bool is_server = isServerSystem(info);

  if (is_server) {
    switch (info.build_version) {
      // Серверные версии
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
        name = info.product_name;
        return name;
    }
  } else {
    switch (info.build_version) {
      // Клиентские версии
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
        name = "Windows 10 (1507)";
        break;
      case WindowsBuild::WIN10_1511:
        name = "Windows 10 (1511)";
        break;
      case WindowsBuild::WIN10_1607:
        name = "Windows 10 (1607)";
        break;
      case WindowsBuild::WIN10_1703:
        name = "Windows 10 (1703)";
        break;
      case WindowsBuild::WIN10_1709:
        name = "Windows 10 (1709)";
        break;
      case WindowsBuild::WIN10_1803:
        name = "Windows 10 (1803)";
        break;
      case WindowsBuild::WIN10_1809:
        name = "Windows 10 (1809)";
        break;
      case WindowsBuild::WIN10_1903:
        name = "Windows 10 (1903)";
        break;
      case WindowsBuild::WIN10_1909:
        name = "Windows 10 (1909)";
        break;
      case WindowsBuild::WIN10_2004:
        name = "Windows 10 (2004)";
        break;
      case WindowsBuild::WIN10_20H2:
        name = "Windows 10 (20H2)";
        break;
      case WindowsBuild::WIN10_21H1:
        name = "Windows 10 (21H1)";
        break;
      case WindowsBuild::WIN10_21H2:
        name = "Windows 10 (21H2)";
        break;
      case WindowsBuild::WIN10_22H2:
        name = "Windows 10 (22H2)";
        break;
      case WindowsBuild::WIN11_21H2:
        name = "Windows 11 (21H2)";
        break;
      case WindowsBuild::WIN11_22H2:
        name = "Windows 11 (22H2)";
        break;
      case WindowsBuild::WIN11_23H2:
        name = "Windows 11 (23H2)";
        break;
      case WindowsBuild::WIN11_24H2:
        name = "Windows 11 (24H2)";
        break;
      default:
        name = info.product_name;
        return name;
    }
  }

  // Добавление информации о сервис-паках для старых версий
  if (!info.release_id.empty() &&
      info.release_id.find("Service Pack") != std::string::npos) {
    name += " " + info.release_id;
  }

  // Добавление редакции системы
  if (!info.edition.empty()) {
    name += " " + info.edition;
  }

  // Уточнение для серверных версий
  if (is_server && name.find("Server") == std::string::npos) {
    name += " Server";
  }

  return name;
}

std::string OSDetection::getStringValue(
    const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>& values,
    const std::string& valueName, const std::string& defaultValue) {
  for (const auto& data : values) {
    const std::string short_name = extractShortName(data->getName());

    if (short_name == valueName) {
      try {
        if (data->getType() == RegistryAnalysis::RegistryValueType::REG_SZ ||
            data->getType() ==
                RegistryAnalysis::RegistryValueType::REG_EXPAND_SZ) {
          return data->getAsString();
        }
      } catch (...) {
        // Обработка ошибок преобразования
      }
    }
  }
  return defaultValue;
}

std::string OSDetection::extractShortName(const std::string& fullName) {
  const size_t pos = fullName.find_last_of("/\\");
  return (pos == std::string::npos) ? fullName : fullName.substr(pos + 1);
}

}
