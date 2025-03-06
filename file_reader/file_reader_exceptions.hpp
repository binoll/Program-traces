#pragma once

#include <stdexcept>
#include <string>
#include <system_error>

/**
 * @defgroup filereader_exceptions Исключения модуля файлового ввода
 * @brief Иерархия исключений для обработки ошибок операций с файлами
 * @ingroup filereader
 */

/**
 * @ingroup filereader_exceptions
 * @brief Базовое исключение для операций с файлами
 * 
 * Содержит информацию о пути к файлу, сообщение об ошибке 
 * и системный код ошибки (если доступен).
 */
class FileException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к файлу, вызвавшему ошибку
   * @param message Описание ошибки
   * @param error_code Код системной ошибки (по умолчанию 0)
   */
  FileException(const std::string& file_path, const std::string& message,
                int error_code = 0)
      : std::runtime_error(formatMessage(file_path, message, error_code)),
        error_code_(error_code) {}

  /**
   * @brief Получение кода системной ошибки
   * @return Целочисленный код ошибки в стиле errno
   */
  [[nodiscard]] int code() const noexcept { return error_code_; }

 private:
  static std::string formatMessage(const std::string& file_path,
                                   const std::string& message, int error_code) {
    std::string result = "[Файл: " + file_path + "] " + message;

    if (error_code != 0) {
      result += " (Код: " + std::to_string(error_code) + " - " +
                std::system_category().message(error_code) + ")";
    }

    return result;
  }

  int error_code_;  ///< Кешированный код системной ошибки
};

/**
 * @ingroup filereader_exceptions
 * @brief Ошибка открытия файла
 */
class FileOpenException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к проблемному файлу
   * @param error_code Код ошибки открытия
   */
  FileOpenException(const std::string& file_path, int error_code)
      : FileException(file_path, "Ошибка открытия файла", error_code) {}
};

/**
 * @ingroup filereader_exceptions
 * @brief Ошибка чтения данных
 */
class FileReadException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к проблемному файлу
   * @param details Детали ошибки чтения
   */
  FileReadException(const std::string& file_path, const std::string& details)
      : FileException(file_path, "Ошибка чтения: " + details) {}
};

/**
 * @ingroup filereader_exceptions
 * @brief Ошибка позиционирования в файле
 */
class FileSeekException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к проблемному файлу
   * @param error_code Код ошибки позиционирования
   */
  FileSeekException(const std::string& file_path, int error_code)
      : FileException(file_path, "Ошибка позиционирования", error_code) {}
};

/**
 * @ingroup filereader_exceptions
 * @brief Ошибка состояния файлового потока
 */
class FileStateException : public FileException {
 public:
  /**
   * @brief Конструктор исключения
   * @param file_path Путь к проблемному файлу
   * @param error_code Код ошибки состояния
   */
  FileStateException(const std::string& file_path, int error_code)
      : FileException(file_path, "Некорректное состояние файла", error_code) {}
};
