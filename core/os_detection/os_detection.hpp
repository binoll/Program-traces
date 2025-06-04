#pragma once

#include <algorithm>
#include <utility>

#include "../../analysis/registry/parser/iparser.hpp"
#include "../../core/exceptions/os_detection_exception.hpp"
#include "../../utils/logging/logger.hpp"
#include "ios_detection.hpp"
#include "os_info.hpp"

namespace WindowsVersion {

/// @class OSDetection
/// @brief Реализация определения версии Windows на основе данных реестра
class OSDetection final : public IOSDetection {
 public:
  /// @name Основные методы класса
  /// @{

  /// @brief Конструктор
  /// @param parser Указатель на парсер реестра
  /// @param softwareFilePath Путь к файлу реестра SOFTWARE
  OSDetection(std::unique_ptr<RegistryAnalysis::IRegistryParser> parser,
              std::string softwareFilePath)
      : parser_(std::move(parser)),
        software_file_path_(std::move(softwareFilePath)) {}

  /// @brief Деструктор по умолчанию
  ~OSDetection() override = default;

  /// @}

  /// @name Методы определения версии ОС
  /// @{

  /// @copydoc IOSDetection::detect
  /// @throw OSDetectionException в случае ошибки определения версии ОС
  [[nodiscard]] OSInfo detect() const override;

  /// @}

 private:
  /// @name Вспомогательные методы для работы с реестром
  /// @{

  /// @brief Извлекает информацию об ОС из значений реестра
  /// @param values Вектор значений реестра
  /// @param[out] info Структура для заполнения данными об ОС
  static void extractOSInfo(
      const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>&
          values,
      OSInfo& info);

  /// @brief Получает строковое значение из реестра
  /// @param values Вектор значений реестра
  /// @param valueName Имя параметра реестра
  /// @param defaultValue Значение по умолчанию, если параметр не найден
  /// @return Значение параметра реестра или значение по умолчанию
  static std::string getStringValue(
      const std::vector<std::unique_ptr<RegistryAnalysis::IRegistryData>>&
          values,
      const std::string& valueName, const std::string& defaultValue = "");

  /// @brief Извлекает короткое имя параметра из полного пути
  /// @param fullName Полный путь к параметру реестра
  /// @return Короткое имя параметра (последний компонент пути)
  static std::string extractShortName(const std::string& fullName);

  /// @}

  /// @name Методы обработки информации об ОС
  /// @{

  /// @brief Определяет точное название ОС на основе собранной информации
  /// @param[in, out] info Структура с данными об ОС (изменяется поле
  /// real_os_name)
  static void determineRealOSName(OSInfo& info);

  /// @brief Получает строковое название ОС на основе информации о версии
  /// @param info Структура с данными об ОС
  /// @return Строка с названием операционной системы
  static std::string getOSName(const OSInfo& info);

  /// @brief Определяет версию Windows по номеру сборки
  /// @param build_number Номер сборки ОС
  /// @param isServer Признак серверной версии ОС
  /// @return Значение перечисления WindowsBuild, соответствующее версии ОС
  static WindowsBuild resolveBuildVersion(int build_number, bool isServer);

  /// @brief Проверяет, является ли система серверной версией Windows
  /// @param info Структура с данными об ОС
  /// @return true если система является серверной версией, false в противном
  /// случае
  static bool isServerSystem(const OSInfo& info);

  /// @}

  std::unique_ptr<RegistryAnalysis::IRegistryParser>
      parser_;                      ///< Парсер реестра
  std::string software_file_path_;  ///< Путь к файлу реестра SOFTWARE
};

}
