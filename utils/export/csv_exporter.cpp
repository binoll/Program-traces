#include "csv_exporter.hpp"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>

#include "../../core/analysis/program_analysis/data/analysis_data.hpp"
#include "../../core/exceptions/csv_export_exception.hpp"

namespace fs = std::filesystem;
using namespace PrefetchAnalysis;

namespace {

struct AggregatedData {
  std::vector<std::string> run_times;
  std::set<std::string> autorun_locations;
  std::vector<WindowsDiskAnalysis::NetworkConnection> network_connections;
  std::vector<VolumeInfo> volumes;
  std::vector<FileMetric> metrics;
  uint32_t run_count = 0;
};

std::string normalizePath(const std::string& path) {
  if (path.empty()) return "";

  std::string result = path;
  std::ranges::transform(result, result.begin(),
                         [](unsigned char c) { return std::tolower(c); });

  std::ranges::replace(result, '/', '\\');

  while (!result.empty() && (result.front() == ' ' || result.front() == '"')) {
    result.erase(0, 1);
  }

  while (!result.empty() && (result.back() == ' ' || result.back() == '"')) {
    result.pop_back();
  }

  return result;
}

std::string getFilenameFromPath(const std::string& path) {
  fs::path p(path);
  return p.filename().string();
}

std::string volumeTypeToString(uint32_t type) {
  switch (static_cast<VolumeType>(type)) {
    case VolumeType::FIXED:
      return "FIXED";
    case VolumeType::REMOVABLE:
      return "REMOVABLE";
    case VolumeType::NETWORK:
      return "NETWORK";
    case VolumeType::OPTICAL:
      return "CDROM";
    case VolumeType::RAMDISK:
      return "RAM";
    case VolumeType::SYSTEM:
      return "SYSTEM";
    case VolumeType::TEMPORARY:
      return "TEMPORARY";
    case VolumeType::VIRTUAL:
      return "VIRTUAL";
    default:
      return "UNKNOWN";
  }
}

std::string accessFlagsToString(uint32_t flags) {
  std::string result;
  if (flags & static_cast<uint32_t>(FileMetricAccess::READ)) result += "READ,";
  if (flags & static_cast<uint32_t>(FileMetricAccess::WRITE))
    result += "WRITE,";
  if (flags & static_cast<uint32_t>(FileMetricAccess::EXECUTE))
    result += "EXECUTE,";
  if (flags & static_cast<uint32_t>(FileMetricAccess::DELETE))
    result += "DELETE,";

  if (!result.empty()) result.pop_back();
  return result;
}

}  // namespace

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
    // Заголовок CSV
    file << "ИсполняемыйФайл,ВременаЗапуска,Автозагрузка,"
         << "СетевыеПодключения,КоличествоЗапусков,"
         << "Тома(серийный:тип:размер),ФайловыеМетрики(имя:размер:доступ)\n";

    // Создаем карту для агрегации данных по нормализованным путям
    std::map<std::string, AggregatedData> aggregated_data;

    // 1. Обрабатываем данные автозагрузки
    for (const auto& entry : autorun_entries) {
      std::string norm_path = normalizePath(entry.path);
      if (norm_path.empty()) continue;

      aggregated_data[norm_path].autorun_locations.insert(entry.location);
    }

    // 2. Обрабатываем данные процессов
    for (const auto& [path, info] : process_data) {
      std::string norm_path = normalizePath(path);
      if (norm_path.empty()) continue;

      auto& data = aggregated_data[norm_path];
      data.run_times = info.run_times;
      data.run_count = info.run_count;
      data.volumes = info.volumes;
      data.metrics = info.metrics;
    }

    // 3. Обрабатываем сетевые подключения
    for (const auto& conn : network_connections) {
      std::string norm_path = normalizePath(conn.process_name);
      if (norm_path.empty()) continue;

      // Создаем запись, если ее нет
      if (!aggregated_data.contains(norm_path)) {
        aggregated_data[norm_path] = AggregatedData();
      }

      aggregated_data[norm_path].network_connections.push_back(conn);
    }

    // 4. Генерируем выходные данные
    for (const auto& [norm_path, data] : aggregated_data) {
      auto escape = [](const std::string& s) {
        std::string result;
        for (char c : s) {
          if (c == '"')
            result += "\"\"";
          else
            result += c;
        }
        return result;
      };

      // Определяем отображаемое имя пути
      std::string display_path = getFilenameFromPath(norm_path);
      if (display_path.empty()) display_path = norm_path;

      // Форматирование времени запуска
      std::stringstream run_times;
      for (size_t i = 0; i < data.run_times.size(); ++i) {
        run_times << data.run_times[i];
        if (i < data.run_times.size() - 1) run_times << ";";
      }

      // Форматирование автозагрузки
      std::stringstream autorun;
      if (!data.autorun_locations.empty()) {
        autorun << "Да(";
        size_t count = 0;
        for (const auto& location : data.autorun_locations) {
          if (count++ > 0) autorun << ", ";
          autorun << location;
        }
        autorun << ")";
      } else {
        autorun << "Нет";
      }

      // Форматирование сетевых подключений
      std::stringstream network;
      for (const auto& conn : data.network_connections) {
        network << conn.protocol << ":" << conn.local_address << "->"
                << conn.remote_address << ":" << conn.port << ";";
      }

      // Форматирование томов
      std::stringstream volumes;
      for (const auto& vol : data.volumes) {
        volumes << vol.getSerialNumber() << ":"
                << volumeTypeToString(vol.getVolumeType()) << ":"
                << vol.getVolumeSize() << " байт;";
      }

      // Форматирование файловых метрик
      std::stringstream metrics;
      for (const auto& metric : data.metrics) {
        fs::path file_path(metric.getFilename());
        metrics << file_path.filename().string() << ":" << metric.getFileSize()
                << " байт:" << accessFlagsToString(metric.getAccessFlags())
                << ";";
      }

      // Запись данных
      file << "\"" << escape(norm_path) << "\","
           << "\"" << escape(run_times.str()) << "\","
           << "\"" << escape(autorun.str()) << "\","
           << "\"" << escape(network.str()) << "\"," << data.run_count << ","
           << "\"" << escape(volumes.str()) << "\","
           << "\"" << escape(metrics.str()) << "\"\n";
    }
  } catch (const std::exception& e) {
    throw CsvExportException(std::string("Ошибка при экспорте данных: ") +
                             e.what());
  }
}

}
