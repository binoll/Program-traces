#include "prefetch_analyzer.hpp"
#include <filesystem>

namespace WindowsDiskAnalysis {

PrefetchAnalyzer::PrefetchAnalyzer(
    std::unique_ptr<PrefetchAnalysis::IPrefetchParser> parser
) : parser_(std::move(parser)) {}

std::map<std::string, ProcessInfo> PrefetchAnalyzer::collect(
    const std::string& disk_root,
    const OSInfo& os_info,
    const Config& config
) {
  std::map<std::string, ProcessInfo> results;

  // Определяем путь к папке Prefetch
  std::string prefetch_path = disk_root + "/Windows/Prefetch";
  if (os_info.is_server) {
    prefetch_path = config.getString("Prefetch", "ServerPath", prefetch_path);
  }
  else if (os_info.current_version == WindowsVersion::WindowsXP) {
    prefetch_path = config.getString("Prefetch", "XP_Path", prefetch_path);
  }

  // Проверяем существование директории
  if (!std::filesystem::exists(prefetch_path)) {
    return results;
  }

  // Обрабатываем все .pf файлы
  for (const auto& entry : std::filesystem::directory_iterator(prefetch_path)) {
    if (entry.path().extension() != ".pf") continue;

    try {
      auto prefetch_data = parser_->parse(entry.path().string());

      ProcessInfo info;
      info.executable_path = prefetch_data->getExecutableName();
      info.run_times = prefetch_data->getRunTimes();
      info.run_count = prefetch_data->getRunCount();

      // Проверяем существование файла
      info.deleted = !std::filesystem::exists(info.executable_path);

      // Сохраняем в результаты
      results[info.executable_path] = info;
    }
    catch (const std::exception& e) {
      // Пропускаем поврежденные файлы
      continue;
    }
  }

  return results;
}

}
