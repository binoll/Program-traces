#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../../../core/exceptions/os_detection_exception.hpp"
#include "../../../parsers/registry/parser/iparser.hpp"
#include "../../../utils/config/config.hpp"
#include "../../../utils/logging/logger.hpp"
#include "ios_detection.hpp"
#include "os_info.hpp"

static std::vector<std::string> split(const std::string& str, char delimiter);

static void trim(std::string& str);

std::string getLastPathComponent(const std::string& path, char separator = '/');

namespace WindowsVersion {

/// @brief Конфигурация для определения версии ОС
struct VersionConfig {
  std::string registry_file;  ///< Путь к файлу реестра
  std::string registry_key;   ///< Путь к ключу в реестре
  std::map<std::string, std::string> registry_values;  ///< Ключи и их имена
  std::vector<std::string> registry_keys;
};

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
  [[nodiscard]] OSInfo detect() override;

 private:
  /// @brief Загружает конфигурации из файла
  void loadConfiguration();

  /// @brief Извлекает информацию об ОС из значений реестра
  void extractOSInfo(
      const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>&
          values,
      OSInfo& info, const std::string& version_name) const;

  /// @brief Определяет точное название ОС
  void determineFullOSName(OSInfo& info) const;

  /// @brief Проверяет серверную версию ОС
  bool isServerSystem(const OSInfo& info) const;

  std::unique_ptr<RegistryAnalysis::IRegistryParser>
      parser_;                    ///< Парсер реестра
  Config config_;                 ///< Конфигурация
  std::string device_root_path_;  ///< Корневой путь устройства
  std::map<std::string, VersionConfig>
      version_configs_;  ///< Конфигурации версий
  std::vector<std::string> default_server_keywords_;
  std::map<uint32_t, std::string> client_builds;
  std::map<uint32_t, std::string> server_builds;
};

}
