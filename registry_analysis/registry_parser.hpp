/**
 * @file registry_parser.hpp
 * @brief Основной интерфейс парсера реестра Windows
 * @details Реализует чтение и анализ файлов реестра с использованием библиотеки libregf.
 *          Использует идиому PIMPL для сокрытия деталей реализации.
*/

#pragma once

#include <memory>
#include <string>
#include "handles.hpp"
#include "interfaces.hpp"
#include "models.hpp"

namespace RegistryAnalysis {

/**
 * @class RegistryParser
 * @brief Основной класс для взаимодействия с файлами реестра
 * @implements IKeyValueProvider
 * @details Реализует интерфейс доступа к данным через PIMPL
*/
class RegistryParser : public IKeyValueProvider {
 public:
  /**
   * @brief Конструктор по умолчанию
   * @throw RegistryInitializationError При ошибке инициализации libregf
  */
  RegistryParser();

  /// @brief Виртуальный деструктор
  ~RegistryParser() override;

  /**
   * @brief Открыть файл реестра для анализа
   * @param[in] file_path Полный путь к файлу реестра
   * @throw FileOpenError При ошибке доступа к файлу
   * @note Поддерживает форматы: NTUSER.DAT, SYSTEM, SOFTWARE и другие
  */
  void open(const std::string& file_path) const;

  /**
   * @brief Получить все значения указанного ключа
   * @param[in] key_path Путь к ключу в формате "ROOT\\Subkey\\..."
   * @return Вектор значений RegistryValue
   * @throw SubkeyNotFoundError Если ключ не существует
   * @throw RootKeyError При проблемах доступа к корневому разделу
  */
  [[nodiscard]] std::vector<RegistryValue> getAllKeyValues(
      const std::string& key_path) const override;

  /**
   * @brief Получить конкретное значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя искомого значения
   * @return Структура RegistryValue с данными
   * @throw ValueNotFoundError Если значение отсутствует
   * @throw BinaryDataReadError При ошибке чтения бинарных данных
  */
  [[nodiscard]] RegistryValue getValueByName(
      const std::string& key_path,
      const std::string& value_name) const override;

 private:
  /**
   * @brief Внутренняя реализация (PIMPL)
   * @details Содержит низкоуровневые операции работы с libregf
  */
  class Impl;

  std::unique_ptr<Impl>
      impl_;  ///< @brief Уникальный указатель на Внутреннюю реализацию (PIMPL)
};

/**
 * @class RegistryParser::Impl
 * @brief Внутренняя реализация функционала анализатора
 * @details Инкапсулирует прямую работу с API libregf
*/
class RegistryParser::Impl {
 public:
  /**
   * @brief Конструктор внутренней реализации
   * @throw RegistryInitializationError При ошибке создания файлового дескриптора
  */
  Impl();

  /// @brief Деструктор
  ~Impl();

  /**
   * @brief Открыть файл реестра
   * @param[in] file_path Путь к файлу реестра
   * @throw FileOpenError При ошибке чтения файла
  */
  void open(const std::string& file_path) const;

  /**
   * @brief Получить все значения ключа
   * @param[in] key_path Иерархический путь к ключу
   * @return Вектор значений
   * @throw SubkeyNotFoundError При невалидном пути
  */
  [[nodiscard]] std::vector<RegistryValue> getAllKeyValues(
      const std::string& key_path) const;

  /**
   * @brief Получить значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя значения
   * @return Запрошенное значение
   * @throw ValueNotFoundError При отсутствии значения
  */
  [[nodiscard]] RegistryValue getValueByName(
      const std::string& key_path, const std::string& value_name) const;

 private:
  /**
   * @brief Разрешить иерархический путь к ключу
   * @param[in] path Путь вида "ROOT\\Subkey1\\Subkey2"
   * @return Дескриптор ключа
   * @throw SubkeyNotFoundError При отсутствии элементов пути
  */
  [[nodiscard]] KeyHandle resolveKeyPath(const std::string& path) const;

  /**
   * @brief Извлечь данные значения реестра
   * @param[in] value Указатель на объект значения
   * @return Структура с данными
   * @throw ValueDataExtractionError При ошибке преобразования
  */
  static RegistryValue extractValueData(libregf_value_t* value);

  /// @brief Извлечь строковое значение (REG_SZ, REG_EXPAND_SZ)
  static void extractString(libregf_value_t* value, std::string& output);

  /// @brief Извлечь 32-битное целое (REG_DWORD)
  static void extractDword(libregf_value_t* value, std::string& output);

  /// @brief Извлечь 64-битное целое (REG_QWORD)
  static void extractQword(libregf_value_t* value, std::string& output);

  /// @brief Извлечь бинарные данные (REG_BINARY)
  static void extractBinary(libregf_value_t* value, std::string& output);

  libregf_file_t* file_ =
      nullptr;  ///< @brief Указатель на файл реестра (libregf)
};

}
