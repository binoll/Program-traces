/// @file parser.hpp
/// @brief Реализация парсера файлов реестра Windows

#pragma once

#include <libregf/types.h>

#include <memory>
#include <string>
#include <vector>

#include "../data_model/data_builder.hpp"
#include "../data_types/key.hpp"
#include "../data_types/value.hpp"
#include "iparser.hpp"

namespace RegistryAnalysis {

/// @class RegistryParser
/// @brief Конкретная реализация парсера реестра на основе libregf
class RegistryParser : public IRegistryParser {
 public:
  /// @name Основные методы класса
  /// @{

  /// @brief Конструктор по умолчанию
  RegistryParser() = default;

  /// @brief Деструктор - автоматически закрывает открытые файлы
  ~RegistryParser() override;

  /// @}

  /// @name Методы получения значений
  /// @{

  /// @copydoc RegistryParser::getKeyValues
  /// @throw RegistryException В случае ошибок доступа или парсинга
  std::vector<std::unique_ptr<IRegistryData>> getKeyValues(
      const std::string& registry_file_path,
      const std::string& registry_key_path) override;

  /// @copydoc RegistryParser::getSpecificValue
  /// @throw RegistryException В случае ошибок доступа или парсинга
  std::unique_ptr<IRegistryData> getSpecificValue(
      const std::string& registry_file_path,
      const std::string& registry_value_path) override;

  /// @}

 private:
  /// @name Внутренние методы работы с файлами
  /// @{

  /// @brief Открыть файл реестра
  /// @param[in] registry_file_path Путь к файлу реестра
  /// @throw FileOpenError При ошибке открытия файла
  void openRegistryFile(const std::string& registry_file_path);

  /// @brief Закрыть текущий открытый файл реестра
  void closeRegistryFile();

  /// @}

  /// @name Методы поиска элементов реестра
  /// @{

  /// @brief Найти раздел реестра по пути
  /// @param[in] key_path Путь к разделу реестра
  /// @return Handle раздела реестра
  /// @throw KeyNotFoundError Если раздел не найден
  [[nodiscard]] KeyHandle findRegistryKey(const std::string& key_path) const;

  /// @brief Найти значение в разделе по имени
  /// @param[in] registry_key Handle раздела реестра
  /// @param[in] value_name Имя значения
  /// @return Handle значения реестра
  /// @throw ValueNotFoundError Если значение не найдено
  static ValueHandle findRegistryValue(libregf_key_t* registry_key,
                                       const std::string& value_name);
  /// @}

  /// @name Методы преобразования данных
  /// @{

  /// @brief Преобразовать тип значения из libregf в внутреннее представление
  /// @param[in] libregf_type Тип значения в формате libregf
  /// @return Тип значения в формате RegistryValueType
  static RegistryValueType convertValueType(uint32_t libregf_type);

  /// @brief Обработать данные значения и заполнить построитель
  /// @param[in] value_handle Handle значения реестра
  /// @param[out] builder Построитель объектов данных
  static void processValueData(libregf_value_t* value_handle,
                               RegistryDataBuilder& builder);

  /// @brief Создать объект данных реестра
  /// @param[in] value_handle Handle значения реестра
  /// @param[in] value_path Полный путь к значению
  /// @return Указатель на созданный объект данных
  static std::unique_ptr<IRegistryData> createRegistryDataObject(
      libregf_value_t* value_handle, const std::string& value_path);

  /// @}

  libregf_file_t* regf_file_handle_ =
      nullptr;  ///< Handle открытого файла реестра
};

}
