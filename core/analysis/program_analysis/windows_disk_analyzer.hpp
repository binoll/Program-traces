#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../../../utils/config/config.hpp"
#include "../../../utils/export/iexporter.hpp"
#include "../os_detection/os_info.hpp"
#include "amcache/amcache_analyzer.hpp"
#include "autorun/autorun_analyzer.hpp"
#include "logs/eventlog_analyzer.hpp"
#include "prefetch/prefetch_analyzer.hpp"
#include "data/analysis_data.hpp"

namespace WindowsDiskAnalysis {

/// @class WindowsDiskAnalyzer
/// @brief Главный класс для анализа диска Windows
/// @details Координирует работу всех анализаторов (Autorun, Prefetch, EventLog,
/// Amcache) и собирает результаты для экспорта.
class WindowsDiskAnalyzer {
 public:
  /// @brief Конструктор анализатора
  /// @param[in] disk_root Корневая директория анализируемого диска
  /// @param[in] config_path Путь к файлу конфигурации
  WindowsDiskAnalyzer(std::string disk_root, const std::string& config_path);

  /// @brief Запускает процесс анализа
  /// @param[in] output_path Путь для сохранения отчета
  /// @param[in] exporter Экспортер данных (опционально, по умолчанию
  /// CSVExporter)
  void analyze(const std::string& output_path,
               std::unique_ptr<IExporter> exporter = nullptr);

 private:
  /// @brief Инициализирует компоненты анализатора
  void initializeComponents();

  /// @brief Определяет версию операционной системы
  void detectOSVersion();

  /// @brief Проверяет существование директории и создает её при необходимости
  /// @param[in] path Путь к файлу или директории
  static void ensureDirectoryExists(const std::string& path);

  std::string disk_root_;    ///< Корневой путь к диску
  std::string config_path_;  ///< Путь к конфигурации
  OSInfo os_info_;           ///< Информация об ОС

  std::unique_ptr<AutorunAnalyzer>
      autorun_analyzer_;  ///< Анализатор автозагрузки
  std::unique_ptr<PrefetchAnalyzer>
      prefetch_analyzer_;  ///< Анализатор Prefetch
  std::unique_ptr<EventLogAnalyzer>
      eventlog_analyzer_;  ///< Анализатор журналов событий
  std::unique_ptr<AmcacheAnalyzer> amcache_analyzer_;  ///< Анализатор Amcache
};

}
