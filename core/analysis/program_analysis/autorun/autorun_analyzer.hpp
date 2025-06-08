#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>

#include "../../../../parsers/registry/parser/parser.hpp"
#include "../../os_detection/ios_detection.hpp"
#include "../data/analysis_data.hpp"

static std::vector<std::string> split(const std::string& str, char delimiter);

static void trim(std::string& str);

namespace WindowsDiskAnalysis {

/// @brief Конфигурация автозапуска для конкретной версии ОС
struct AutorunConfig {
  std::string registry_path;                    ///< Путь к файлу куста реестра
  std::vector<std::string> registry_locations;  ///< Пути в реестре
  std::vector<std::string> filesystem_paths;    ///< Пути в файловой системе
};

/// @brief Анализатор записей автозапуска
class AutorunAnalyzer {
 public:
  /// @brief Конструктор
  /// @param parser Парсер реестра
  /// @param os_version Версия ОС
  /// @param ini_path Путь к конфигурационному файлу
  AutorunAnalyzer(std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
                  std::string os_version, const std::string& ini_path);

  /// @brief Сбор записей автозапуска
  /// @param disk_root Корень диска для анализа
  /// @return Вектор записей автозапуска
  std::vector<AutorunEntry> collect(const std::string& disk_root);

 private:
  void loadConfigurations(const std::string& ini_path);
  std::vector<AutorunEntry> analyzeRegistry(const std::string& disk_root);
  std::vector<AutorunEntry> analyzeFilesystem(const std::string& disk_root);
  static void processWildcardPath(const std::string& disk_root,
                                  const std::string& path,
                                  std::vector<AutorunEntry>& results);
  static AutorunEntry createFilesystemEntry(
      const std::filesystem::path& file_path, const std::string& location);

  std::unique_ptr<RegistryAnalysis::IRegistryParser> parser_;
  std::map<std::string, AutorunConfig> configs_;
  std::string os_version_;
};

}
