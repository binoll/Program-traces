#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>

#include "analysis/logs/evt/parser/parser.hpp"
#include "utils/logging/logger.hpp"

using namespace EventLogAnalysis;
using namespace std::chrono;

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  const auto logger = GlobalLogger::get();

  if (argc < 2) {
    logger->error("Usage: {} <path_to_directory>", argv[0]);
    return 1;
  }

  try {
    // Проверка существования директории
    fs::path dir_path = argv[1];
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
      logger->error(
          "Указанный путь не существует или не является директорией: {}",
          dir_path.string());
      return 1;
    }

    logger->info("Анализ журналов событий в директории: {}", dir_path.string());

    // Счетчики для сводной статистики
    size_t total_files = 0;
    size_t total_events = 0;

    // 1. Инициализация парсера журналов событий
    EvtParser parser;

    // 2. Проход по всем файлам в директории
    for (const auto& entry : fs::directory_iterator(dir_path)) {
      if (!entry.is_regular_file()) continue;
      if (entry.path().filename().extension() != ".Evt") continue;

      const fs::path& evtx_path = entry.path();
      logger->info("\nОбработка файла: {}", evtx_path.filename().string());

      try {
        // 3. Открытие EVTX-файла
        total_files++;

        // 4. Получение событий запуска программ
        auto process_events = parser.getEventsByType(evtx_path, 4688);
        if (process_events.empty()) {
          logger->info("  Найдено {} событий Sysmon (ID 1)",
                       process_events.size());
        } else {
          logger->info("  Найдено {} событий безопасности (ID 4688)",
                       process_events.size());
        }

        total_events += process_events.size();

        // 5. Обработка и вывод результатов
        for (const auto& event : process_events) {
          const auto& data = event->getData();

          // Извлечение временной метки
          std::string timestamp_str = "N/A";
          uint64_t filetime = event->getTimestamp();  // Получаем значение FILETIME
          if (filetime > 0) {
            // Конвертируем 100-наносекундные интервалы в наносекунды
            auto duration_since_epoch =
                nanoseconds((filetime - 116444736000000000ULL) * 100);

            // Создаем временную точку system_clock
            system_clock::time_point timestamp(duration_since_epoch);

            // Конвертируем в time_t
            time_t time = system_clock::to_time_t(timestamp);

            // Форматируем время
            std::tm tm = *std::gmtime(&time);  // Используем UTC время
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            std::string timestamp_str = oss.str();

            // Используем timestamp_str в выводе
            std::cout << "[Время: " << timestamp_str << "]";
          }

          // Извлечение основной информации
          std::string user = data.count("SubjectUserName")
                                 ? data.at("SubjectUserName")
                             : data.count("User") ? data.at("User")
                                                  : "N/A";

          std::string process = data.count("NewProcessName")
                                    ? data.at("NewProcessName")
                                : data.count("Image") ? data.at("Image")
                                                      : "N/A";

          std::string cmd_line =
              data.count("CommandLine") ? data.at("CommandLine") : "N/A";

          std::string parent =
              data.count("ParentProcessName") ? data.at("ParentProcessName")
              : data.count("ParentImage")     ? data.at("ParentImage")
                                              : "N/A";

          // Форматированный вывод
          std::cout << "\n[Файл: " << evtx_path.filename().string() << "]";
          std::cout << "\n[Время: " << timestamp_str << "]";
          std::cout << "\nПроцесс: " << process;
          std::cout << "\nПользователь: " << user;
          std::cout << "\nКомандная строка: " << cmd_line;
          std::cout << "\nРодительский процесс: " << parent;

          // Дополнительные поля
          if (data.count("Company")) {
            std::cout << "\nПроизводитель: " << data.at("Company");
          }
          if (data.count("IntegrityLevel")) {
            std::cout << "\nУровень целостности: " << data.at("IntegrityLevel");
          }
          std::cout << "\n---------------------------------\n";
        }

      } catch (const std::exception& e) {
        logger->error("  Ошибка обработки файла {}: {}",
                      evtx_path.filename().string(), e.what());
      }
    }

    // Сводная статистика
    logger->info("\n=== Сводка ===");
    logger->info("Обработано файлов: {}", total_files);
    logger->info("Найдено событий запуска: {}", total_events);
    logger->info("=========================");

    return 0;
  } catch (const std::exception& e) {
    logger->error("Критическая ошибка: {}", e.what());
    return 1;
  }
}
