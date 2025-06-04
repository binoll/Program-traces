#include <iostream>

#include "analysis/registry/parser/parser.hpp"
#include "core/exceptions/os_detection_exception.hpp"
#include "core/os_detection/os_detection.hpp"
#include "utils/logging/logger.hpp"

using namespace RegistryAnalysis;
using namespace WindowsVersion;

int main(int argc, char* argv[]) {
  const auto logger = GlobalLogger::get();

  try {
    // 1. Инициализация парсера реестра
    auto parser = std::make_unique<RegistryParser>();

    // 2. Путь к файлу SOFTWARE
    std::string registry_file = argv[1];

    logger->info("Запуск определения версии Windows");
    logger->debug("Используется файл реестра: {}", registry_file);

    // 3. Создание детектора ОС
    OSDetection detector(std::move(parser), registry_file);

    // 4. Определение версии ОС
    OSInfo os_info = detector.detect();

    // 5. Вывод результатов
    std::cout << "\n=== Информация об операционной системе ===\n";
    std::cout << "Операционная система: " << os_info.real_os_name << "\n";
    std::cout << "Операционная система: " << os_info.product_name << "\n";
    std::cout << "Версия ОС: " << os_info.version << "\n";
    std::cout << "Номер сборки: " << os_info.build_number << "\n";

    if (!os_info.edition.empty()) {
      std::cout << "Редакция: " << os_info.edition << "\n";
    }

    if (!os_info.release_id.empty()) {
      std::cout << "Идентификатор выпуска: " << os_info.release_id << "\n";
    }

    if (!os_info.display_version.empty()) {
      std::cout << "Отображаемая версия: " << os_info.display_version << "\n";
    }

    std::cout << "======================================\n";

    return 0;
  } catch (const std::exception& e) {
    logger->error("Непредвиденная ошибка: {}", e.what());
  }

  return 1;
}
