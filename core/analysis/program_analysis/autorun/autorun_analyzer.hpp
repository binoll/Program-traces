/// @file autorun_analyzer.hpp
/// @brief Анализатор автозагружаемых элементов Windows

#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>

#include "../../../../parsers/registry/parser/parser.hpp"
#include "../../os_detection/ios_detection.hpp"
#include "../data/analysis_data.hpp"

/// @brief Разделяет строку на подстроки по указанному разделителю
/// @param str Входная строка для разделения
/// @param delimiter Символ-разделитель
/// @return Вектор подстрок
static std::vector<std::string> split(const std::string& str, char delimiter);

/// @brief Удаляет пробельные символы с начала и конца строки
/// @param str Строка для обработки (модифицируется на месте)
static void trim(std::string& str);

namespace WindowsDiskAnalysis {

/// @brief Анализатор автозагружаемых элементов Windows
/// @details Выполняет поиск программ, запускаемых автоматически при загрузке
/// системы, анализируя как записи реестра, так и файловую систему
class AutorunAnalyzer {
 public:
  /// @brief Конструктор анализатора
  /// @param parser Экземпляр парсера реестра для работы с кустами
  /// @param os_version Версия целевой ОС (должна соответствовать секции в INI)
  /// @param ini_path Путь к конфигурационному файлу с параметрами автозапуска
  AutorunAnalyzer(std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
                  std::string os_version, const std::string& ini_path);

  /// @brief Основной метод сбора данных об автозапуске
  /// @param disk_root Корневой путь анализируемого диска (точка монтирования)
  /// @return Вектор обнаруженных записей автозапуска
  std::vector<AutorunEntry> collect(const std::string& disk_root);

 private:
  /// @brief Загружает конфигурации из INI-файла
  /// @param ini_path Путь к конфигурационному файлу
  void loadConfigurations(const std::string& ini_path);

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
      parser_;  ///< Парсер для работы с кустами реестра
  std::map<std::string, IniConfig>
      configs_;             ///< Конфигурации для различных версий ОС
  std::string os_version_;  ///< Целевая версия ОС для анализа
};

}
