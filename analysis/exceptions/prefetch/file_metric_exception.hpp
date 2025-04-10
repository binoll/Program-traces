/**
 * @file file_metric_exception.hpp
 * @brief Исключения, связанные с обработкой файловых метрик.
 * @details Этот файл содержит исключения для ошибок, возникающих при обработке метрик файлов.
*/

#pragma once

#include <stdexcept>
#include <string>

namespace PrefetchAnalysis {

/**
 * @class FileMetricException
 * @brief Базовый класс для ошибок работы с файловыми метриками.
 * @details Все исключения, связанные с обработкой метрик файлов, наследуются от этого класса.
*/
class FileMetricException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения для ошибок работы с файловыми метриками.
   * @param message Описание ошибки.
   * @details Этот конструктор передает сообщение об ошибке в базовый класс std::runtime_error.
  */
  explicit FileMetricException(const std::string& message)
      : std::runtime_error(message) {}

  /**
   * @brief Деструктор.
   * @details Переопределенный деструктор для корректной очистки.
  */
  ~FileMetricException() noexcept override = default;
};

/**
 * @class InvalidFileMetricException
 * @brief Исключение для некорректных параметров файла.
 * @details Генерируется, если:
 *          - Нарушен формат имени файла.
 *          - Обнаружена недопустимая ссылка на MFT.
*/
class InvalidFileMetricException : public FileMetricException {
 public:
  /**
   * @brief Конструктор исключения для некорректных параметров файла.
   * @param context Контекст ошибки для диагностики.
   * @details Конструктор формирует подробное сообщение об ошибке с контекстом.
  */
  explicit InvalidFileMetricException(const std::string& context)
      : FileMetricException("Некорректные параметры файла: " + context) {}
};

}
