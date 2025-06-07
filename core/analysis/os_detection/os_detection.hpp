#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../../core/exceptions/os_detection_exception.hpp"
#include "../../parsers/registry/parser/iparser.hpp"
#include "../../utils/config/config.hpp"
#include "../../utils/logging/logger.hpp"
#include "ios_detection.hpp"
#include "os_info.hpp"

namespace WindowsVersion {

/// @class OSDetection
/// @brief Реализация определения версии Windows на основе данных реестра
class OSDetection final : public IOSDetection {
 public:
  /// @brief Конструктор
  /// @param parser Указатель на парсер реестра
  /// @param config Конфигурация для определения версий
  /// @param device_root_path Путь к корневой директории устройства (например,
  /// "C:")
  OSDetection(std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
              Config&& config, std::string device_root_path);

  ~OSDetection() override = default;

  /// @brief Определяет версию Windows
  /// @return Информация об операционной системе
  /// @throw OSDetectionException при ошибке определения версии
  [[nodiscard]] OSInfo detect() const override;

 private:
  /// @brief Конфигурация для определения версии ОС
  struct VersionConfig {
    std::string registry_file;  ///< Путь к файлу реестра
    std::string registry_key;   ///< Путь к ключу в реестре
    std::map<std::string, std::string> registry_values;  ///< Ключи и их имена
  };

  /// @brief Загружает конфигурации из файла
  void loadConfiguration();

  /// @brief Извлекает информацию об ОС из значений реестра
  void extractOSInfo(
      const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>&
          values,
      OSInfo& info, const std::string& version_name) const;

  /// @brief Извлекает последний компонент из пути реестра
  static std::string getLastComponent(const std::string& path);

  /// @brief Определяет точное название ОС
  static void determineRealOSName(OSInfo& info);

  /// @brief Проверяет серверную версию ОС
  static bool isServerSystem(const OSInfo& info);

  /// @brief Определяет версию сборки
  static WindowsBuild resolveBuildVersion(int build_number, bool is_server);

  /// @brief Формирует читаемое имя ОС
  static std::string getOSName(const OSInfo& info);

  std::unique_ptr<RegistryAnalysis::IRegistryParser>
      parser_;                    ///< Парсер реестра
  Config config_;                 ///< Конфигурация
  std::string device_root_path_;  ///< Корневой путь устройства
  std::map<std::string, VersionConfig>
      version_configs_;  ///< Конфигурации версий
};

}
