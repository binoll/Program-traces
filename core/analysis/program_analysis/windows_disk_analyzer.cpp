#include "windows_disk_analyzer.hpp"

#include <filesystem>

#include "../../../parsers/logs/evt/parser/parser.hpp"
#include "../../../parsers/logs/evtx/parser/parser.hpp"
#include "../os_detection/os_detection.hpp"

namespace fs = std::filesystem;
using namespace WindowsDiskAnalysis;

WindowsDiskAnalyzer::WindowsDiskAnalyzer(const std::string& disk_root,
                                         const std::string& config_path)
    : disk_root_(disk_root), config_path_(config_path) {
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
  autorun_analyzer_ = std::make_unique<AutorunAnalyzer>(
      std::move(registry_parser), os_info_.ini_version, config_path_);

  prefetch_analyzer_ = std::make_unique<PrefetchAnalyzer>(
      std::move(prefetch_parser), os_info_.ini_version, config_path_);

  eventlog_analyzer_ = std::make_unique<EventLogAnalyzer>(
      std::move(evt_parser), std::move(evtx_parser), os_info_.ini_version,
      config_path_);
}

void WindowsDiskAnalyzer::ensureDirectoryExists(const std::string& path) {
  const fs::path dir_path = fs::path(path).parent_path();
  if (!dir_path.empty() && !exists(dir_path)) {
    create_directories(dir_path);
  }
}

void WindowsDiskAnalyzer::analyze(const std::string& output_path) {
  // 1. Сбор данных об автозагрузке
  autorun_entries_ = autorun_analyzer_->collect(disk_root_);

  // 2. Сбор данных из Prefetch
  auto prefetch_results = prefetch_analyzer_->collect(disk_root_);
  for (auto& info : prefetch_results) {
    process_data_[info.filename] = std::move(info);
  }

  // 3. Анализ журналов событий
  eventlog_analyzer_->collect(disk_root_, process_data_, network_connections_);

  // 5. Экспорт результатов
  ensureDirectoryExists(output_path);
  CSVExporter::exportToCSV(output_path, autorun_entries_, process_data_,
                           network_connections_);
}