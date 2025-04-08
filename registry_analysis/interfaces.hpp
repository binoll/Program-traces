/**
 * @file interfaces.hpp
 * @brief Интерфейсы для работы с данными реестра
 * @details Определение контрактов для доступа к ключам и значениям реестра.
*/

#pragma once

#include <string>
#include <vector>
#include "exceptions.hpp"
#include "models.hpp"

namespace RegistryAnalysis {

/**
 * @interface IKeyValueProvider
 * @brief Интерфейс для доступа к данным реестра
 * @details Определяет контракт для чтения ключей и значений реестра.
*/
class IKeyValueProvider {
 public:
  /// @brief Виртуальный деструктор интерфейса
  virtual ~IKeyValueProvider() = default;

  /**
   * @brief Получить все значения ключа
   * @param[in] key_path Путь к ключу в формате "ROOT\\Подраздел\\..."
   * @return Вектор значений типа RegistryValue
   * @throw SubkeyNotFoundError Если подраздел не найден
   * @throw RootKeyError Если корневой ключ недоступен
  */
  [[nodiscard]] virtual std::vector<RegistryValue> getAllKeyValues(
      const std::string& key_path) const = 0;

  /**
   * @brief Получить значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя значения (регистрозависимое)
   * @return Структура RegistryValue с данными
   * @throw ValueNotFoundError Если значение отсутствует
   * @throw SubkeyNotFoundError Если ключ не существует
  */
  [[nodiscard]] virtual RegistryValue getValueByName(
      const std::string& key_path, const std::string& value_name) const = 0;
};

}
