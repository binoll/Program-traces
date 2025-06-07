#include "windows_disk_analyzer.hpp"
#include "../../../parsers/registry/parser/parser.hpp"
#include "../../../parsers/prefetch/parser/parser.hpp"
#include "../../../parsers/logs/evtx/parser/parser.hpp"
#include "../../../parsers/logs/evt/parser/parser.hpp"
#include "../../../utils/logging/logger.hpp"
#include "../../../utils/export/csv_exporter.hpp"

namespace WindowsDiskAnalysis {

WindowsDiskAnalyzer::WindowsDiskAnalyzer(const std::string& disk_root, const std::string& config_path)
    : disk_root_(disk_root), config_(config_path) {
    initializeComponents();
    detectOSVersion();
}

void WindowsDiskAnalyzer::initializeComponents() {
    autorun_analyzer_ = std::make_unique<AutorunAnalyzer>(
        std::make_unique<RegistryAnalysis::RegistryParser>()
    );
    
    prefetch_analyzer_ = std::make_unique<PrefetchAnalyzer>(
        std::make_unique<PrefetchAnalysis::PrefetchParser>()
    );
    
    eventlog_analyzer_ = std::make_unique<EventLogAnalyzer>(
        (os_info_.current_version == WindowsVersion::WindowsXP) ? 
        std::unique_ptr<EventLogAnalysis::IEventLogParser>(
            std::make_unique<EventLogAnalysis::EvtParser>()
        ) : 
        std::unique_ptr<EventLogAnalysis::IEventLogParser>(
            std::make_unique<EventLogAnalysis::EvtxParser>()
        )
    );
    
    file_analyzer_ = std::make_unique<FileAnalyzer>();
}

void WindowsDiskAnalyzer::detectOSVersion() {
    auto os_detector = std::make_unique<WindowsVersion::OSDetection>(
        std::make_unique<RegistryAnalysis::RegistryParser>(), 
        Config(config_),
        disk_root_
    );
    os_info_ = os_detector->detect();
}

void WindowsDiskAnalyzer::analyze(const std::string& output_path) {
    // Сбор данных
    autorun_entries_ = autorun_analyzer_->collect(disk_root_);
    process_data_ = prefetch_analyzer_->collect(disk_root_, os_info_, config_);
    eventlog_analyzer_->collect(disk_root_, os_info_, process_data_, network_connections_);
    file_analyzer_->collect(process_data_);
    
    // Экспорт
    CSVExporter::exportToCSV(output_path, autorun_entries_, process_data_, network_connections_);
}

}
