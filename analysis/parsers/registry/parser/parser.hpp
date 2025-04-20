/**
 * @file parser.hpp
 * @brief Основная реализация парсера реестра Windows
 */

#pragma once

#include <libregf.h>

#include <string>
#include <vector>

#include "../../../exceptions/general/parsing_exception.hpp"
#include "../../../exceptions/registry/registry_exception.hpp"
#include "../handle/key.hpp"
#include "iparser.hpp"

namespace RegistryAnalysis {

/**
 * @class RegistryParser
 * @brief Класс для анализа и извлечения значений из файлов реестра Windows
 * @note Предоставляет методы для открытия файлов реестра, извлечения
 * значений ключей и конкретных значений по имени. Работает с библиотекой
 * libregf
 */
class RegistryParser : public IRegistryParser {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор по умолчанию
   * @throw InitializationError Если не удалось инициализировать libregf
   */
  RegistryParser();

  /**
   * @brief Деструктор
   * @note Освобождает ресурсы, связанные с libregf
   */
  ~RegistryParser() override;
  /// @}

  /**
   * @brief Открывает файл реестра для последующего анализа
   * @param[in] file_path Абсолютный путь к файлу реестра
   * @throw FileOpenException Если файл не удалось открыть
   */
  void open(const std::string& file_path) override;

  /**
   * @brief Получает все значения, содержащиеся в указанном ключе.
   * @param[in] key_path Путь к ключу, например: "ROOT\\Software\\Microsoft".
   * @return Вектор значений ключа.
   * @throw SubkeyNotFoundError Если указанный путь не существует.
   */
  [[nodiscard]] std::vector<Value> getAllKeyValues(
      const std::string& key_path) const override;

  /**
   * @brief Извлекает конкретное значение по имени из указанного ключа.
   * @param[in] key_path Путь к ключу, содержащему значение.
   * @param[in] value_name Имя искомого значения.
   * @return Объект Value, содержащий данные.
   * @throw ValueNotFoundError Если значение не найдено.
   */
  [[nodiscard]] Value getValueByName(
      const std::string& key_path,
      const std::string& value_name) const override;

 private:
  /**
   * @brief Преобразует строковый путь в объект KeyHandle.
   * @param[in] path Путь к ключу в реестре.
   * @return Объект KeyHandle, соответствующий указанному пути.
   * @throw SubkeyNotFoundError Если путь недопустим.
   */
  [[nodiscard]] KeyHandle resolveKeyPath(const std::string& path) const;

  /**
   * @brief Извлекает структуру Value из указателя на значение libregf.
   * @param[in] value Указатель на значение из библиотеки libregf.
   * @return Объект Value с данными.
   * @throw ValueDataExtractionError При ошибке извлечения данных.
   */
  static Value extractValueData(libregf_value_t* value);

  /**
   * @brief Извлекает строковое значение из объекта libregf_value_t.
   * @param[in] value Указатель на значение.
   * @param[out] output Результирующая строка.
   * @throw BinaryDataReadError При ошибке чтения строки.
   */
  static void extractString(libregf_value_t* value, std::string& output);

  /**
   * @brief Извлекает значение DWORD из объекта libregf_value_t.
   * @param[in] value Указатель на значение.
   * @param[out] output Строковое представление DWORD.
   * @throw BinaryDataReadError При ошибке чтения значения.
   */
  static void extractDword(libregf_value_t* value, std::string& output);

  /**
   * @brief Извлекает значение QWORD из объекта libregf_value_t.
   * @param[in] value Указатель на значение.
   * @param[out] output Строковое представление QWORD.
   * @throw BinaryDataReadError При ошибке чтения значения.
   */
  static void extractQword(libregf_value_t* value, std::string& output);

  /**
   * @brief Извлекает бинарные данные из объекта libregf_value_t.
   * @param[in] value Указатель на значение.
   * @param[out] output Бинарные данные в виде строки.
   * @throw BinaryDataReadError При ошибке чтения данных.
   */
  static void extractBinary(libregf_value_t* value, std::string& output);

  /**
   * @brief Указатель на файл реестра, открытый через libregf.
   */
  libregf_file_t* file_;
};

}
