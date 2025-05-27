/**
 * @file registry_value.hpp
 * @brief Модель данных значения реестра Windows
 * @note Определяет структуру для представления значения в реестре Windows,
 *          включая имя, данные и тип значения
 */

#pragma once

#include <cstdint>
#include <string>

namespace RegistryAnalysis {

/**
 * @enum RegistryValueType
 * @brief Типы данных значений реестра Windows
 */
enum class RegistryValueType : uint32_t {
  REG_NONE = 0,              ///< Не определено
  REG_SZ = 1,                ///< Строка с завершающим нулём
  REG_EXPAND_SZ = 2,         ///< Строка с переменными окружения
  REG_BINARY = 3,            ///< Бинарные данные
  REG_DWORD = 4,             ///< 32-битное целое
  REG_DWORD_BIG_ENDIAN = 5,  ///< 32-битное целое (обратный порядок)
  REG_LINK = 6,              ///< Символическая ссылка
  REG_MULTI_SZ = 7,          ///< Массив строк с двойным нулём
  REG_RESOURCE_LIST = 8,     ///< Список ресурсов
  REG_QWORD = 11             ///< 64-битное целое
};

/**
 * @struct Value
 * @brief Представление значения реестра Windows
 */
struct Value {
  /// @name Основные свойства
  /// @{
  std::string
      name;  ///< Имя значения (UTF-8). Пустая строка для значения по умолчанию
  std::string data;        ///< Строковое представление данных
  RegistryValueType type;  ///< Тип значения из перечисления RegistryValueType
  /// @}

  /// @name Вспомогательные методы
  /// @{
  /**
   * @brief Получает человеко-читаемое описание типа
   */
  [[nodiscard]] std::string typeDescription() const {
    switch (type) {
      case RegistryValueType::REG_SZ:
        return "String";
      case RegistryValueType::REG_EXPAND_SZ:
        return "Expandable String";
      case RegistryValueType::REG_BINARY:
        return "Binary Data";
      case RegistryValueType::REG_DWORD:
        return "32-bit Integer";
      case RegistryValueType::REG_QWORD:
        return "64-bit Integer";
      case RegistryValueType::REG_MULTI_SZ:
        return "Multi-string";
      default:
        return "Unknown Type";
    }
  }

  /**
   * @brief Проверяет является ли значение строковым типом
   */
  [[nodiscard]] bool isStringType() const {
    return type == RegistryValueType::REG_SZ ||
           type == RegistryValueType::REG_EXPAND_SZ ||
           type == RegistryValueType::REG_MULTI_SZ;
  }
  /// @}
};

}
