/// @file iexporter.hpp
/// @brief Интерфейс для экспорта результатов анализа
/// @details Определяет контракт для классов, выполняющих экспорт собранных данных в различные форматы

#pragma once

#include <string>
#include <vector>
#include <map>

#include "../../core/analysis/program_analysis/data/analysis_data.hpp"

namespace WindowsDiskAnalysis {

/// @class IExporter
/// @brief Интерфейс для экспорта данных анализа
class IExporter {
public:
    /// @brief Виртуальный деструктор
    virtual ~IExporter() = default;

    /// @brief Экспортирует данные анализа
    /// @param[in] output_path Путь к выходному файлу
    /// @param[in] autorun_entries Список записей автозагрузки
    /// @param[in] process_data Данные о процессах
    /// @param[in] network_connections Список сетевых подключений
    /// @param[in] amcache_entries Записи Amcache
    virtual void exportData(
        const std::string& output_path,
        const std::vector<AutorunEntry>& autorun_entries,
        const std::map<std::string, ProcessInfo>& process_data,
        const std::vector<NetworkConnection>& network_connections,
        const std::vector<AmcacheEntry>& amcache_entries) = 0;
};

}
