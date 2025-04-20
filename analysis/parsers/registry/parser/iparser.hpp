/**
 * @file iparser.hpp
 * @brief Интерфейс для работы с реестром Windows
 */

#pragma once

#include <string>
#include <vector>

#include "../model/registry_value.hpp"

namespace RegistryAnalysis {

/**
 * @interface IRegistryParser
 * @brief Базовый интерфейс парсера реестра Windows
 * @note Реализует паттерн "Фасад", предоставляя унифицированный доступ
 *          к данным реестра независимо от формата хранения (HIV, REG и др.)
 */
class IRegistryParser {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Виртуальный деструктор по умолчанию
   * @note Гарантирует корректное разрушение производных классов
   */
  virtual ~IRegistryParser() noexcept = default;
  /// @}

  /// @name Получение данных
  /// @{
  /**
   * @brief Открытие файла реестра
   * @param[in] file_path Абсолютный путь к файлу реестра
   * @note Поддерживаемые форматы:
   * - Бинарные файлы HIVE (Windows Registry Hive)
   * - Текстовые файлы .REG (Экспорт реестра)
   */
  virtual void open(const std::string& file_path) = 0;

  /**
   * @brief Получение всех значений ключа
   * @param[in] key_path Путь в формате "ROOT\\Subkey\\..."
   * @return Вектор значений типа Value
   * @note Для корневого ключа используйте "ROOT\\"
   */
  [[nodiscard]] virtual std::vector<Value> getAllKeyValues(
      const std::string& key_path) const = 0;

  /**
   * @brief Поиск значения по имени
   * @param[in] key_path   Путь к родительскому ключу
   * @param[in] value_name Имя искомого значения (регистрозависимое)
   * @return Значение типа Value
   */
  [[nodiscard]] virtual Value getValueByName(
      const std::string& key_path, const std::string& value_name) const = 0;
  /// @}
};

}
