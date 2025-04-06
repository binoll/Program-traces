#include <iostream>
#include "registry_analysis/registry_analyzer.hpp"
#include "registry_analysis/registry_analyzer_exceptions.hpp"

int main(int argc, char* argv[]) {
  try {
    RegistryAnalysis::RegistryAnalyzer analyzer;

    // Путь к SOFTWARE (а не SAM)
    std::string software_path(argv[1]);
    software_path += "/Windows/System32/config/SOFTWARE";

    analyzer.Open(software_path);
    std::cout << "Файл SOFTWARE успешно открыт\n";

    // Ключ, содержащий информацию о версии Windows
    const std::string version_key = "Microsoft/Windows NT/CurrentVersion";

    // Получаем все значения ключа
    auto values = analyzer.GetAllKeyValues(version_key);

    std::cout << "\nИнформация о версии Windows:\n";
    for (const auto& val : values) {
      if (val.name == "ProductName" || val.name == "CurrentVersion" ||
          val.name == "CurrentBuild" || val.name == "DisplayVersion") {
        std::cout << val.name << ": " << val.data << "\n";
      }
    }

  } catch (const RegistryAnalysis::FileOpenError& e) {
    std::cerr << "Ошибка открытия файла: " << e.what() << "\n";
    return 1;
  } catch (const RegistryAnalysis::SubkeyNotFoundError& e) {
    std::cerr << "Ошибка поиска ключа: " << e.what() << "\n";
    return 2;
  } catch (const std::exception& e) {
    std::cerr << "Неизвестная ошибка: " << e.what() << "\n";
    return 3;
  }

  return 0;
}