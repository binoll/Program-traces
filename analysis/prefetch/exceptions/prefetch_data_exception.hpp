/**
 * @file prefetch_data_exception.hpp
 * @brief Исключения для обработки данных Prefetch.
 * @details Этот файл содержит исключения для обработки ошибок, возникающих при анализе данных Prefetch.
*/

#pragma once

#include <stdexcept>
#include <string>

namespace PrefetchAnalysis {

/**
 * @brief Базовый класс для ошибок, связанных с обработкой данных Prefetch.
 * @details Этот класс является базой для всех исключений, которые происходят при анализе данных Prefetch.
*/
class PrefetchDataException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения для ошибок обработки данных Prefetch.
   * @param message Описание ошибки.
   * @details Этот конструктор передает сообщение об ошибке в базовый класс std::runtime_error.
  */
  explicit PrefetchDataException(const std::string& message)
      : std::runtime_error(message) {}

  /**
   * @brief Деструктор.
   * @details Переопределенный деструктор для корректной очистки.
  */
  ~PrefetchDataException() noexcept override = default;
};

/**
 * @brief Исключение для некорректного имени исполняемого файла.
 * @details Генерируется при:
 *          - Пустом имени файла.
 *          - Наличии запрещенных символов в имени.
 *          - Превышении допустимой длины имени.
*/
class InvalidExecutableNameException : public PrefetchDataException {
 public:
  /**
   * @brief Конструктор исключения для некорректного имени исполняемого файла.
   * @param name Некорректное имя исполняемого файла.
   * @details Конструктор формирует подробное сообщение об ошибке с именем файла.
  */
  explicit InvalidExecutableNameException(const std::string& name)
      : PrefetchDataException("Недопустимое имя исполняемого файла: " + name) {}
};

}
