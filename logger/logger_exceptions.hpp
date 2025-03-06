#pragma once

#include <stdexcept>
#include <string>

/**
 * @brief Исключения логирования
 * @ingroup logger
 */
class LoggerException : public std::runtime_error {
 public:
  explicit LoggerException(const std::string& message)
      : std::runtime_error("[Логгер] " + message) {}
};

/// @brief Ошибка инициализации логгера
class LoggerInitException : public LoggerException {
 public:
  explicit LoggerInitException(const std::string& details)
      : LoggerException("Не удалось выполнить инициализацию: " + details) {}
};

/// @brief Ошибка записи в лог
class LoggerWriteException : public LoggerException {
 public:
  explicit LoggerWriteException(const std::string& details)
      : LoggerException("Не удалось выполнить операцию записи: " + details) {}
};
