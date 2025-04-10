#include <ctime>
#include <iomanip>
#include <iostream>
#include "analysis/prefetch/parser/prefetch_parser.hpp"

// Helper function to convert Windows FILETIME to time_t
time_t filetime_to_timet(uint64_t filetime) {
  if (filetime == 0) {
    return 0;
  }
  // Convert 100-ns intervals to seconds since 1601-01-01
  filetime /= 10000000ULL;
  // Subtract difference between 1601 and 1970 (11644473600 seconds)
  return static_cast<time_t>(filetime - 11644473600ULL);
}

void printPrefetchInfo(const PrefetchAnalysis::IPrefetchData& info) {
  std::cout << "Executable: " << info.getExecutableName() << "\n"
            << "Prefetch Hash: 0x" << std::hex << info.getPrefetchHash() << "\n"
            << "Run Count: " << std::dec << info.getRunCount() << "\n\n"
            << "Last Run Times:\n";

  for (const auto& time : info.getRunTimes()) {
    std::cout << "  "
              << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
              << "\n";
  }

  std::cout << "\nVolumes:\n";
  for (const auto& vol : info.getVolumes()) {
    time_t creation_time = filetime_to_timet(vol.getCreationTime());
    std::cout << "  Device: " << vol.getDevicePath() << "\n"
              << "  Serial: 0x" << std::hex << vol.getSerialNumber() << "\n"
              << "  Created: "
              << std::put_time(std::localtime(&creation_time),
                               "%Y-%m-%d %H:%M:%S")
              << "\n\n";
  }

  std::cout << "Tracked Files:\n";
  for (const auto& metric : info.getMetrics()) {
    std::cout << "  " << metric.getFilename() << " [Ref: 0x" << std::hex
              << metric.getFileReference() << "]\n";
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <prefetch_file>\n";
    return 1;
  }

  try {
    // Создание объекта парсера Prefetch
    PrefetchAnalysis::PrefetchParser parser;

    // Парсинг данных из файла и получение уникального указателя на IPrefetchData
    std::unique_ptr<PrefetchAnalysis::IPrefetchData> info =
        parser.parse(argv[1]).build();

    // Вывод информации о Prefetch
    printPrefetchInfo(*info);
  } catch (const std::exception& e) {
    // Обработка ошибок
    std::cerr << "Error: " << e.what() << "\n";
    return 2;
  }

  return 0;
}
