#include "windows_disk_analyzer.hpp"

#include <filesystem>
#include <utility>

#include "../../../parsers/event_log/formats/evt/parser.hpp"
#include "../../../parsers/event_log/formats/evtx/parser.hpp"
#include "../../../parsers/registry/parser/parser.hpp"
#include "../../../utils/export/csv_exporter.hpp"
#include "../../../utils/logging/logger.hpp"
#include "../os_detection/os_detection.hpp"

namespace fs = std::filesystem;
using namespace WindowsDiskAnalysis;

WindowsDiskAnalyzer::WindowsDiskAnalyzer(std::string disk_root,
                                         const std::string& config_path)
    : disk_root_(std::move(disk_root)), config_path_(config_path) {
  const auto logger = GlobalLogger::get();

  logger->info("Загрузка конфигурации из файла: \"{}\"", config_path);
  detectOSVersion();
  initializeComponents();
}

void WindowsDiskAnalyzer::detectOSVersion() {
  std::unique_ptr<RegistryAnalysis::IRegistryParser> registry_parser =
      std::make_unique<RegistryAnalysis::RegistryParser>();
  Config config(config_path_);

  WindowsVersion::OSDetection detector((std::move(registry_parser)),
                                       (std::move(config)), disk_root_);
  os_info_ = detector.detect();
}

void WindowsDiskAnalyzer::initializeComponents() {
  // Инициализация парсеров
  auto registry_parser = std::make_unique<RegistryAnalysis::RegistryParser>();
  auto prefetch_parser = std::make_unique<PrefetchAnalysis::PrefetchParser>();
  auto evt_parser = std::make_unique<EventLogAnalysis::EvtParser>();
  auto evtx_parser = std::make_unique<EventLogAnalysis::EvtxParser>();

  // Создание анализаторов
  auto autorun_config =
      AutorunAnalyzer::createConfig(config_path_, os_info_.ini_version);
  autorun_analyzer_ = std::make_unique<AutorunAnalyzer>(
      std::move(registry_parser), autorun_config);

  prefetch_analyzer_ = std::make_unique<PrefetchAnalyzer>(
      std::move(prefetch_parser), os_info_.ini_version, config_path_);

  eventlog_analyzer_ = std::make_unique<EventLogAnalyzer>(
      std::move(evt_parser), std::move(evtx_parser), os_info_.ini_version,
      config_path_);

  // Добавленная инициализация AmcacheAnalyzer
  auto amcache_registry_parser =
      std::make_unique<RegistryAnalysis::RegistryParser>();
  amcache_analyzer_ = std::make_unique<AmcacheAnalyzer>(
      std::move(amcache_registry_parser), os_info_.ini_version, config_path_);
}

void WindowsDiskAnalyzer::ensureDirectoryExists(const std::string& path) {
  const fs::path dir_path = fs::path(path).parent_path();
  if (!dir_path.empty() && !exists(dir_path)) {
    create_directories(dir_path);
  }
}

void WindowsDiskAnalyzer::analyze(const std::string& output_path,
                                  std::unique_ptr<IExporter> exporter) {
  AnalysisResult result;

  // 1. Сбор данных об автозагрузке
  result.autorun_entries = autorun_analyzer_->collect(disk_root_);

  // 2. Сбор данных из Amcache.hve
  result.amcache_entries = amcache_analyzer_->collect(disk_root_);

  // 3. Сбор данных из Prefetch
  auto prefetch_results = prefetch_analyzer_->collect(disk_root_);
  for (auto& info : prefetch_results) {
    result.process_data[info.filename] = std::move(info);
  }

  // 4. Анализ журналов событий
  eventlog_analyzer_->collect(disk_root_, result.process_data,
                              result.network_connections);

  // 5. Экспорт результатов
  ensureDirectoryExists(output_path);

  if (!exporter) {
    exporter = std::make_unique<CSVExporter>();
  }

  exporter->exportData(output_path, result.autorun_entries, result.process_data,
                       result.network_connections, result.amcache_entries);
}
