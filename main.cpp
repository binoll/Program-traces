#include <iostream>

#include "analysis/parsers/registry/parser/parser.hpp"
#include "utils/logger/logger.hpp"

int main(int argc, char* argv[]) {
  try {

    RegistryAnalysis::RegistryParser analyzer;
    std::string software_path(argv[1]);
    software_path += "/Windows/System32/config/SOFTWARE";
    analyzer.open(software_path);

    const std::string version_key = "Microsoft/Windows NT/CurrentVersion";
    auto values = analyzer.getAllKeyValues(version_key);

    // Сбор данных
    std::string product_name_, display_version, current_build;
    int major_version = 0, minor_version = 0;

    for (const auto& val : values) {
      if (val.name_ == "ProductName")
        product_name_ = val.data_;
      else if (val.name_ == "DisplayVersion")
        display_version = val.data_;
      else if (val.name_ == "CurrentBuild")
        current_build = val.data_;
      else if (val.name_ == "CurrentMajorVersionNumber")
        major_version = std::stoi(val.data_);
      else if (val.name_ == "CurrentMinorVersionNumber")
        minor_version = std::stoi(val.data_);
    }

    // Определение версии
    bool is_windows11 = (major_version >= 10 && minor_version >= 0 &&
                         std::stoi(current_build) >= 22000);

    std::cout << "Информация о системе:\n"
              << "  ProductName: " << product_name_ << "\n"
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
