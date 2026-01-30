/// @file csv_exporter.hpp
/// @brief Класс для экспорта результатов анализа Windows в CSV формат

#pragma once

#include <map>
#include <string>
#include <vector>

#include "../../core/analysis/program_analysis/data/analysis_data.hpp"
#include "../../core/exceptions/csv_export_exception.hpp"
#include "iexporter.hpp"

namespace WindowsDiskAnalysis {

/// @class CSVExporter
/// @brief Класс для экспорта результатов анализа Windows в CSV формат
/// @details Реализует интерфейс IExporter для сохранения данных в формате CSV.
/// Поддерживает русскоязычные заголовки и форматирование данных.
class CSVExporter : public IExporter {
 public:
  /// @brief Экспортирует данные анализа в CSV файл
  /// @param[in] output_path Путь для сохранения CSV файла
  /// @param[in] autorun_entries Список записей автозагрузки
  /// @param[in] process_data Данные о процессах (путь, хэш, время запусков)
  /// @param[in] network_connections Список сетевых подключений
  /// @param[in] amcache_entries Записи Amcache
  /// @throw CsvExportException В случае ошибок экспорта
  /// @throw FileOpenException Если не удалось открыть файл для записи
  /// @throw DataFormatException При обнаружении некорректных данных
  void exportData(
      const std::string& output_path,
      const std::vector<AutorunEntry>& autorun_entries,
      const std::map<std::string, ProcessInfo>& process_data,
      const std::vector<NetworkConnection>& network_connections,
      const std::vector<AmcacheEntry>& amcache_entries) override;

  /// @brief Статический метод для удобства вызова (совместимость)
  static void exportToCSV(
      const std::string& output_path,
      const std::vector<AutorunEntry>& autorun_entries,
      const std::map<std::string, ProcessInfo>& process_data,
      const std::vector<NetworkConnection>& network_connections,
      const std::vector<AmcacheEntry>& amcache_entries);
};

}
