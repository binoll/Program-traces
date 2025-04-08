#include <iostream>
#include "registry/registry_analyzer.hpp"

int main(int argc, char* argv[]) {
  try {
    RegistryAnalysis::RegistryParser analyzer;
    std::string software_path(argv[1]);
    software_path += "/Windows/System32/config/SOFTWARE";
    analyzer.open(software_path);

    const std::string version_key = "Microsoft/Windows NT/CurrentVersion";
    auto values = analyzer.GetAllKeyValues(version_key);

    // Сбор данных
    std::string product_name, display_version, current_build;
    int major_version = 0, minor_version = 0;

    for (const auto& val : values) {
      if (val.name == "ProductName")
        product_name = val.data;
      else if (val.name == "DisplayVersion")
        display_version = val.data;
      else if (val.name == "CurrentBuild")
        current_build = val.data;
      else if (val.name == "CurrentMajorVersionNumber")
        major_version = std::stoi(val.data);
      else if (val.name == "CurrentMinorVersionNumber")
        minor_version = std::stoi(val.data);
    }

    // Определение версии
    bool is_windows11 = (major_version >= 10 && minor_version >= 0 &&
                         std::stoi(current_build) >= 22000);

    std::cout << "Информация о системе:\n"
              << "  ProductName: " << product_name << "\n"
              << "  DisplayVersion: " << display_version << "\n"
              << "  CurrentBuild: " << current_build << "\n"
              << "  Версия ОС: " << (is_windows11 ? "Windows 11" : "Windows 10")
              << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Ошибка: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
