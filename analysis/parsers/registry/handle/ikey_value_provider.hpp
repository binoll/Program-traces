/**
 * @file ikey_value_provider.hpp
 * @brief Интерфейсы для работы с данными реестра Windows
 */

#pragma once

#include <string>
#include <vector>

#include "../../../exceptions/registry/registry_exception.hpp"
#include "../model/registry_value.hpp"

namespace RegistryAnalysis {

/**
 * @interface IKeyValueProvider
 * @brief Базовый интерфейс для чтения данных системного реестра
 * @note Реализует паттерн "Поставщик данных", абстрагируя доступ
 *          к реестру. Позволяет получать значения ключей реестра независимо
 *          от конкретной реализации доступа (реальный реестр, моки и т.д.)
 */
class IKeyValueProvider {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Виртуальный деструктор по умолчанию
   */
  virtual ~IKeyValueProvider() = default;
  /// @}

  /// @name Методы работы с реестром
  /// @{
  /**
   * @brief Получение всех значений указанного ключа реестра
   * @param[in] key_path Путь к ключу в формате "ROOT\\Подраздел\\..."
   * @return Вектор объектов Value с данными значений
   * @throw RootKeyError Некорректный корневой раздел
   * @throw SubkeyNotFoundError Ключ не существует
   * @warning Регистр разделов имеет значение в Windows
   */
  [[nodiscard]] virtual std::vector<Value> getAllKeyValues(
      const std::string& key_path) const = 0;

  /**
   * @brief Получение конкретного значения по имени
   * @param[in] key_path Путь к целевому ключу реестра
   * @param[in] value_name Имя параметра (регистрозависимое)
   * @return Запрошенное значение реестра
   * @throw SubkeyNotFoundError Ключ не существует
   * @throw ValueNotFoundError Параметр не найден
   */
  [[nodiscard]] virtual Value getValueByName(
      const std::string& key_path, const std::string& value_name) const = 0;
  /// @}
};

}
