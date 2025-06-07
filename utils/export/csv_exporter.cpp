#include "csv_exporter.hpp"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "../../core/analysis/program_analysis/data/analysis_data.hpp"
#include "../../core/exceptions/csv_export_exception.hpp"

namespace WindowsDiskAnalysis {

void CSVExporter::exportToCSV(
    const std::string& output_path,
    const std::vector<AutorunEntry>& autorun_entries,
    const std::map<std::string, ProcessInfo>& process_data,
    const std::vector<NetworkConnection>& network_connections) {
  std::ofstream file(output_path);
  if (!file.is_open()) {
    throw FileOpenException(output_path);
  }

  try {
    file << "Исполняемый путь,Хэш,Времена запуска,Автозагрузка,Версия "
            "ПО,Сетевые подключения,"
         << "Командная строка,Время создания,Время изменения,Количество "
            "запусков\n";

    for (const auto& [path, info] : process_data) {
      // Проверка обязательных полей
      if (path.empty()) {
        throw DataFormatException("Исполняемый путь");
      }

      // Форматирование времени запуска
      std::stringstream run_times;
      for (size_t i = 0; i < info.run_times.size(); ++i) {
        // Конвертация uint64_t в time_t
        auto raw_time = static_cast<time_t>(info.run_times[i]);

        // Безопасное преобразование времени
        tm time_info{};
        if (localtime_r(&raw_time, &time_info)) {
          char buffer[20];
          strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_info);
          run_times << buffer;
        } else {
          throw DataFormatException("Некорректное время запуска");
        }

        if (i < info.run_times.size() - 1) run_times << ";";
      }

      // Поиск в автозагрузке
      std::string autorun_status = "Нет";
      for (const auto& entry : autorun_entries) {
        if (entry.path == path) {
          if (entry.location.empty()) {
            throw DataFormatException("Расположение автозагрузки");
          }
          autorun_status = "Да(" + entry.location + ")";
          break;
        }
      }

      // Сетевые подключения
      std::stringstream network;
      for (const auto& conn : network_connections) {
        if (conn.process_name == path) {
          if (conn.protocol.empty() || conn.local_address.empty()) {
            throw DataFormatException("Сетевые параметры");
          }
          network << conn.protocol << ":" << conn.local_address << "->"
                  << conn.remote_address << ":" << conn.port << ";";
        }
      }

      // Форматирование времени
      std::string creation_time_str = "N/A";
      if (info.creation_time > 0) {
        tm time_info{};
        if (localtime_r(&info.creation_time, &time_info)) {
          char buffer[20];
          strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_info);
          creation_time_str = buffer;
        }
      }

      std::string modification_time_str = "N/A";
      if (info.modification_time > 0) {
        tm time_info{};
        if (localtime_r(&info.modification_time, &time_info)) {
          char buffer[20];
          strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_info);
          modification_time_str = buffer;
        }
      }

      // Запись данных
      file << "\"" << path << "\","
           << "\"" << (info.hash.empty() ? "N/A" : info.hash) << "\","
           << "\"" << run_times.str() << "\","
           << "\"" << autorun_status << "\","
           << "\"" << "" << "\","  // Версия ПО
           << "\"" << network.str() << "\","
           << "\"" << "" << "\","  // Командная строка
           << creation_time_str << "," << modification_time_str << ","
           << info.run_count << "\n";
    }
  } catch (const std::exception& e) {
    throw CsvExportException(std::string("Ошибка при экспорте данных: ") +
                             e.what());
  }
}

}
