#include <iomanip>
#include <iostream>
#include <sstream>

#include "analysis/registry/parser/parser.hpp"
#include "utils/logging/logger.hpp"

using namespace RegistryAnalysis;

// Вспомогательная функция для получения строкового представления значения
std::string valueToString(const IRegistryData* data) {
  switch (data->getType()) {
    case RegistryValueType::REG_SZ:
    case RegistryValueType::REG_EXPAND_SZ:
      return data->getAsString();
    case RegistryValueType::REG_DWORD:
      return std::to_string(data->getAsDword());
    case RegistryValueType::REG_QWORD:
      return std::to_string(data->getAsQword());
    case RegistryValueType::REG_MULTI_SZ: {
      auto strings = data->getAsMultiString();
      std::ostringstream oss;
      for (const auto& s : strings) {
        oss << s << "; ";
      }
      return oss.str();
    }
    case RegistryValueType::REG_BINARY: {
      auto binary = data->getAsBinary();
      std::ostringstream oss;
      oss << "Binary (" << binary.size() << " bytes): ";
      for (size_t i = 0; i < std::min<size_t>(binary.size(), 4); ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(binary[i]) << " ";
      }
      if (binary.size() > 4) oss << "...";
      return oss.str();
    }
    default:
      return "Unsupported type";
  }
}

int main(int argc, char* argv[]) {
  const auto logger = GlobalLogger::get();

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <windows_directory>\n";
    return 1;
  }

  try {
    RegistryParser analyzer;
    std::string software_path = argv[1];
    software_path += "/Windows/System32/config/SOFTWARE";

    const std::string version_key = "Microsoft/Windows NT/CurrentVersion";

    auto values = analyzer.getKeyValues(software_path, version_key);

    // Сбор данных
    std::string product_name, display_version, current_build;
    int major_version = 0, minor_version = 0;

    for (const auto& val_ptr : values) {
      // Извлекаем имя значения из полного пути
      std::string full_path = val_ptr->getName();
      const size_t pos = full_path.find_last_of("/\\");
      std::string value_name =
          (pos == std::string::npos) ? full_path : full_path.substr(pos + 1);

      std::string value_data = valueToString(val_ptr.get());

      if (value_name == "ProductName") {
        product_name = value_data;
      } else if (value_name == "DisplayVersion") {
        display_version = value_data;
      } else if (value_name == "CurrentBuild") {
        current_build = value_data;
      } else if (value_name == "CurrentMajorVersionNumber") {
        try {
          major_version = std::stoi(value_data);
        } catch (...) {
        }
      } else if (value_name == "CurrentMinorVersionNumber") {
        try {
          minor_version = std::stoi(value_data);
        } catch (...) {
        }
      }
    }

    // Определение версии
    bool is_windows11 = false;
    try {
      int build_number = std::stoi(current_build);
      is_windows11 =
          (major_version >= 10 && minor_version >= 0 && build_number >= 22000);
    } catch (...) {
    }

    std::cout << "System Information:\n"
              << "  ProductName: " << product_name << "\n"
              << "  DisplayVersion: " << display_version << "\n"
              << "  CurrentBuild: " << current_build << "\n"
              << "  OS Version: "
              << (is_windows11 ? "Windows 11" : "Windows 10") << "\n";

  } catch (std::exception& e) {
    logger->error(e.what());
    return 1;
  }
  return 0;
}
