#include "prefetch_analyzer.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "../../../../utils/config/config.hpp"
#include "../../../../utils/logging/logger.hpp"
#include "../../../../utils/utils.hpp"

namespace WindowsDiskAnalysis {

PrefetchAnalyzer::PrefetchAnalyzer(
    std::unique_ptr<PrefetchAnalysis::IPrefetchParser> parser,
    std::string os_version, const std::string& ini_path)
    : parser_(std::move(parser)), os_version_(std::move(os_version)) {
  loadConfigurations(ini_path);
}

void PrefetchAnalyzer::loadConfigurations(const std::string& ini_path) {
  Config config(ini_path);
  const auto logger = GlobalLogger::get();

  // Загрузка пути к папке Prefetch
  std::string path = config.getString(os_version_, "PrefetchPath", "");
  trim(path);
  if (!path.empty()) {
    std::replace(path.begin(), path.end(), '\\', '/');
    config_.prefetch_path = path;
  } else {
    // Default path if not specified in config
    config_.prefetch_path = "/Windows/Prefetch";
  }

  logger->debug("Загружена конфигурация Prefetch для \"{}\": путь = \"{}\"",
                os_version_, config_.prefetch_path);
}

std::vector<ProcessInfo> PrefetchAnalyzer::collect(
    const std::string& disk_root) {
  std::vector<ProcessInfo> results;
  const auto logger = GlobalLogger::get();

  std::string prefetch_path = disk_root + config_.prefetch_path;

  // Проверяем существование директории
  if (!std::filesystem::exists(prefetch_path)) {
    logger->warn("Папка Prefetch не найдена: \"{}\"", prefetch_path);
    return results;
  }

  // Обрабатываем все .pf файлы
  size_t processed_count = 0;
  for (const auto& entry : std::filesystem::directory_iterator(prefetch_path)) {
    if (entry.path().extension() != ".pf") continue;

    try {
      auto prefetch_data = parser_->parse(entry.path().string());

      ProcessInfo info;
      info.run_count = prefetch_data->getRunCount();
      info.filename = prefetch_data->getExecutableName();
      info.volumes = prefetch_data->getVolumes();
      info.metrics = prefetch_data->getMetrics();

      // TODO: Add run times conversion if needed, currently just storing empty or
      // raw if available in parser interface. Assuming parser returns raw times
      // and we need to convert them.
      // For now, let's assume we just need to store the info.
      // If conversion is needed, it should be done here.

      // Сохраняем в результаты
      results.emplace_back(std::move(info));
      processed_count++;
    } catch (const std::exception& e) {
      logger->warn("Ошибка анализа файла \"{}\": \"{}\"", entry.path().string(),
                   e.what());
    }
  }

  logger->info(
      "Проанализировано \"{}\" Prefetch-файлов, найдено \"{}\" процессов",
      processed_count, results.size());
  return results;
}

}
