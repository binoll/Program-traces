/**
 * @file iregistry_parser.hpp
 * @brief Интерфейс для парсера реестра Windows.
 * @details Определяет контракт доступа к значениям ключей реестра.
*/

#pragma once

#include <string>
#include <vector>
#include "../model/registry_value.hpp"

namespace RegistryAnalysis {

/**
 * @interface IRegistryParser
 * @brief Интерфейс взаимодействия с парсером реестра.
*/
class IRegistryParser {
 public:
  virtual ~IRegistryParser() = default;

  /**
   * @brief Открыть файл реестра.
   * @param file_path Полный путь к файлу.
  */
  virtual void open(const std::string& file_path) = 0;

  /**
   * @brief Получить все значения ключа.
   * @param key_path Путь вида "ROOT/Subkey/...".
   * @return Вектор значений ключа.
  */
  [[nodiscard]] virtual std::vector<Value> getAllKeyValues(
      const std::string& key_path) const = 0;

  /**
   * @brief Получить конкретное значение по имени.
   * @param key_path Путь к ключу.
   * @param value_name Имя значения.
   * @return Структура Value.
  */
  [[nodiscard]] virtual Value getValueByName(
      const std::string& key_path, const std::string& value_name) const = 0;
};

}
