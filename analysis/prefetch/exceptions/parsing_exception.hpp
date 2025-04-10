/**
 * @file parsing_exception.hpp
 * @brief Исключения для ошибок парсинга данных.
 * @details Этот файл содержит исключения, связанные с ошибками парсинга.
*/

#pragma once

#include <stdexcept>
#include <string>

namespace PrefetchAnalysis {

/**
 * @brief Базовый класс для ошибок парсинга данных.
 * @details Этот класс является базой для всех исключений, которые происходят на разных этапах парсинга.
*/
class ParsingException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения для ошибок парсинга.
   * @param message Описание ошибки.
   * @details Этот конструктор передает сообщение об ошибке в базовый класс std::runtime_error.
  */
  explicit ParsingException(const std::string& message)
      : std::runtime_error("Ошибка парсинга: " + message) {}

  /**
   * @brief Деструктор.
   * @details Переопределенный деструктор для корректной очистки.
  */
  ~ParsingException() noexcept override = default;
};

/**
 * @brief Исключение для ошибок открытия файла.
 * @details Генерируется при:
 *          - Отсутствии файла по указанному пути.
 *          - Отсутствии прав доступа.
 *          - Повреждении файловой системы.
*/
class FileOpenException : public ParsingException {
 public:
  /**
   * @brief Конструктор исключения для ошибок открытия файла.
   * @param path Путь к файлу, который не удалось открыть.
   * @details Конструктор формирует сообщение об ошибке, добавляя путь к проблемному файлу.
  */
  explicit FileOpenException(const std::string& path)
      : ParsingException("Не удалось открыть файл: " + path) {}
};

/**
 * @brief Исключение для некорректного формата данных.
 * @details Генерируется при нарушении структуры данных в файле.
*/
class InvalidFormatException : public ParsingException {
 public:
  /**
   * @brief Конструктор исключения для некорректного формата данных.
   * @param details Технические детали ошибки.
   * @details Конструктор формирует подробное сообщение об ошибке с деталями формата.
  */
  explicit InvalidFormatException(const std::string& details)
      : ParsingException("Некорректный формат данных: " + details) {}
};

/**
 * @brief Исключение для ошибок чтения данных.
 * @details Возникает при сбоях операций ввода-вывода.
*/
class DataReadException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения для ошибок чтения данных.
   * @param context Контекст ошибки.
   * @details Конструктор формирует сообщение об ошибке, добавляя контекст операции.
  */
  explicit DataReadException(const std::string& context)
      : std::runtime_error("Ошибка чтения данных: " + context) {}
};

/**
 * @brief Исключение для некорректной временной метки.
 * @details Генерируется при:
 *          - Времени до эпохи UNIX (01.01.1970).
 *          - Времени, превышающем максимально возможное значение (> 30828 год).
*/
class InvalidTimestampException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения для некорректной временной метки.
   * @param details Технические детали ошибки.
   * @details Конструктор формирует подробное сообщение об ошибке с деталями временной метки.
  */
  explicit InvalidTimestampException(const std::string& details)
      : std::runtime_error("Некорректная временная метка: " + details) {}
};

}
