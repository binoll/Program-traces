#pragma once

#include <stdexcept>
#include <string>
#include <system_error>

/**
 * @defgroup exceptions File Reader Exceptions
 * @brief Иерархия исключений для обработки ошибок операций с файлами
 * @ingroup filereader
 */

/**
 * @ingroup exceptions
 * @brief Базовое исключение для всех файловых операций
 * @details Содержит детализированную информацию об ошибке:
 * - Путь к проблемному файлу
 * - Человекочитаемое сообщение
 * - Системный код ошибки (если доступен)
 */
class FileException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к файлу, вызвавшему ошибку
   * @param message Описание ошибки
   * @param error_code Системный код ошибки (по умолчанию 0)
   *
   * @code
   * throw FileException("data.txt", "Неизвестная ошибка", EBADF);
   * @endcode
   */
  FileException(const std::string& file_path, const std::string& message,
                int error_code = 0)
      : std::runtime_error(formatMessage(file_path, message, error_code)),
        error_code_(error_code) {}

  /**
   * @brief Получение кода системной ошибки
   * @return Целочисленный код ошибки (errno-совместимый)
   */
  [[nodiscard]] int code() const noexcept { return error_code_; }

 private:
  static std::string formatMessage(const std::string& file_path,
                                   const std::string& message, int error_code) {
    std::string result = "[Файл: " + file_path + "] " + message;

    if (error_code != 0) {
      result += " (Системная ошибка: " + std::to_string(error_code) + " - " +
                std::system_category().message(error_code) + ")";
    }

    return result;
  }

  int error_code_;  ///< Кешированный код системной ошибки
};

/**
 * @ingroup exceptions
 * @brief Ошибка открытия файла
 * @details Возникает при неудачной попытке открыть файл
 */
class FileOpenException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к файлу
   * @param error_code Код системной ошибки открытия
   */
  FileOpenException(const std::string& file_path, int error_code)
      : FileException(file_path, "Ошибка открытия файла", error_code) {}
};

/**
 * @ingroup exceptions
 * @brief Ошибка чтения данных
 * @details Возникает при:
 * - Попытке чтения за пределами файла
 * - Физических ошибках чтения
 * - Несоответствии формата данных
 */
class FileReadException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к файлу
   * @param details Дополнительное описание ошибки
   *
   * @code
   * throw FileReadException("data.bin", "Неверная сигнатура файла");
   * @endcode
   */
  FileReadException(const std::string& file_path, const std::string& details)
      : FileException(file_path, "Ошибка чтения: " + details) {}
};

/**
 * @ingroup exceptions
 * @brief Ошибка позиционирования в файле
 * @details Возникает при:
 * - Попытке установки позиции за пределами файла
 * - Ошибках seek-операций
 */
class FileSeekException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к файлу
   * @param error_code Код системной ошибки
   */
  FileSeekException(const std::string& file_path, int error_code)
      : FileException(file_path, "Ошибка позиционирования", error_code) {}
};

/**
 * @ingroup exceptions
 * @brief Некорректное состояние файла
 * @details Возникает при:
 * - Операциях с закрытым файлом
 * - Использовании невалидного дескриптора
 * - Внутренних противоречиях состояния
 */
class FileStateException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к файлу
   * @param error_code Код системной ошибки
   */
  FileStateException(const std::string& file_path, int error_code)
      : FileException(file_path, "Некорректное состояние файла", error_code) {}
};
