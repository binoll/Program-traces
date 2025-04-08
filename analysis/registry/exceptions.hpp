/**
 * @file exceptions.hpp
 * @brief Пользовательские исключения для парсера реестра Windows
 * @details Содержит иерархию классов исключений для обработки ошибок при работе с реестром.
*/

#pragma once

#include <stdexcept>
#include <string>

namespace RegistryParser {

/**
 * @defgroup Exceptions
 * @brief Иерархия классов для обработки ошибок операций с реестром
*/

/**
 * @class Exception
 * @ingroup Exceptions
 * @brief Базовый класс для всех исключений анализатора реестра
 * @details Наследуется от std::runtime_error. Содержит стандартизированное сообщение об ошибке.
*/
class Exception : public std::runtime_error {
 public:
  /**
   * @brief Конструктор с формированием сообщения
   * @param[in] msg Текст ошибки в кодировке UTF-8
  */
  explicit Exception(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @class InitializationError
 * @ingroup Exceptions
 * @brief Ошибка инициализации библиотеки libregf
 * @details Возникает при неудачной попытке создания объекта файла реестра.
*/
class InitializationError : public Exception {
 public:
  InitializationError()
      : Exception("Ошибка инициализации объекта файла реестра") {}
};

/**
 * @class FileOpenError
 * @ingroup Exceptions
 * @brief Ошибка открытия файла реестра
 * @details Возникает при невозможности прочитать файл реестра.
*/
class FileOpenError : public Exception {
 public:
  /**
   * @brief Конструктор с указанием проблемного файла
   * @param[in] path Абсолютный или относительный путь к файлу
  */
  explicit FileOpenError(const std::string& path)
      : Exception("Ошибка открытия файла: " + path) {}
};

/**
 * @class RootKeyError
 * @ingroup Exceptions
 * @brief Ошибка доступа к корневому ключу
 * @details Возникает при неудачной попытке получить корневой ключ файла реестра.
*/
class RootKeyError : public Exception {
 public:
  RootKeyError() : Exception("Ошибка получения корневого ключа") {}
};

/**
 * @class SubkeyNotFoundError
 * @ingroup Exceptions
 * @brief Ошибка поиска подраздела реестра
 * @details Возникает при отсутствии указанного подраздела в пути.
*/
class SubkeyNotFoundError : public Exception {
 public:
  /**
   * @brief Конструктор с именем отсутствующего подраздела
   * @param[in] name Имя подраздела в формате UTF-8
  */
  explicit SubkeyNotFoundError(const std::string& name)
      : Exception("Подраздел '" + name + "' не найден") {}
};

/**
 * @class ValueNotFoundError
 * @ingroup Exceptions
 * @brief Ошибка поиска значения реестра
 * @details Возникает при отсутствии указанного значения в ключе.
*/
class ValueNotFoundError : public Exception {
 public:
  /**
   * @brief Конструктор с именем отсутствующего значения
   * @param[in] name Имя значения в формате UTF-8
  */
  explicit ValueNotFoundError(const std::string& name)
      : Exception("Значение '" + name + "' не найдено") {}
};

/**
 * @class BinaryDataReadError
 * @ingroup Exceptions
 * @brief Ошибка чтения бинарных данных
 * @details Возникает при сбое в процессе чтения бинарных данных значения.
*/
class BinaryDataReadError : public Exception {
 public:
  BinaryDataReadError() : Exception("Ошибка чтения бинарных данных") {}
};

/**
 * @class ValueDataExtractionError
 * @ingroup Exceptions
 * @brief Ошибка извлечения данных значения
 * @details Возникает при невозможности преобразовать данные значения в требуемый формат.
*/
class ValueDataExtractionError : public Exception {
 public:
  ValueDataExtractionError() : Exception("Ошибка извлечения данных значения") {}
};

}
