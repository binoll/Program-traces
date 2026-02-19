/// @file autorun_analyzer.hpp
/// @brief Анализатор автозагружаемых элементов Windows

#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>

#include "../../../../parsers/registry/parser/parser.hpp"
#include "../data/analysis_data.hpp"

namespace WindowsDiskAnalysis {

/// @brief Конфигурация параметров для конкретной версии ОС
struct AutorunConfig {
  std::string
      registry_path;  ///< Абсолютный путь к файлу куста реестра на диске
  std::vector<std::string> registry_locations;  ///< Список ключей реестра
  std::vector<std::string> filesystem_paths;    ///< Пути в файловой системе
};

/// @brief Анализатор автозагружаемых элементов Windows
/// @details Выполняет поиск программ, запускаемых автоматически при загрузке
/// системы, анализируя как записи реестра, так и файловую систему
class AutorunAnalyzer {
 public:
  /// @brief Конструктор анализатора
  /// @param parser Экземпляр парсера реестра для работы с кустами
  /// @param config Конфигурация автозапуска
  AutorunAnalyzer(std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
                  AutorunConfig config);

  /// @brief Создает конфигурацию на основе INI файла и версии ОС
  /// @param ini_path Путь к INI файлу
  /// @param os_version Версия ОС
  /// @return Сформированная конфигурация
  static AutorunConfig createConfig(const std::string& ini_path,
                                    const std::string& os_version);

  /// @brief Основной метод сбора данных об автозапуске
  /// @param disk_root Корневой путь анализируемого диска (точка монтирования)
  /// @return Вектор обнаруженных записей автозапуска
  std::vector<AutorunEntry> collect(const std::string& disk_root);

 private:
  /// @brief Анализирует записи реестра для обнаружения автозапуска
  /// @param disk_root Корневой путь анализируемого диска
  /// @return Вектор записей автозапуска из реестра
  std::vector<AutorunEntry> analyzeRegistry(const std::string& disk_root);

  /// @brief Анализирует файловую систему для обнаружения автозапуска
  /// @param disk_root Корневой путь анализируемого диска
  /// @return Вектор записей автозапуска из файловой системы
  std::vector<AutorunEntry> analyzeFilesystem(const std::string& disk_root);

  /// @brief Обрабатывает пути с wildcard (*) для поиска файлов автозапуска
  /// @param disk_root Корневой путь анализируемого диска
  /// @param path Шаблон пути с wildcard
  /// @param results Вектор результатов (заполняется найденными записями)
  static void processWildcardPath(const std::string& disk_root,
                                  const std::string& path,
                                  std::vector<AutorunEntry>& results);

  /// @brief Создает запись автозапуска на основе файла
  /// @param file_path Полный путь к файлу
  /// @param location Исходный путь из конфигурации
  /// @return Запись автозапуска
  static AutorunEntry createFilesystemEntry(
      const std::filesystem::path& file_path, const std::string& location);

  std::unique_ptr<RegistryAnalysis::IRegistryParser>
      parser_;          ///< Парсер для работы с кустами реестра
  AutorunConfig config_;  ///< Конфигурация для текущей версии ОС
};

}
