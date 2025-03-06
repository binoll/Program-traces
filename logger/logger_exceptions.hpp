#pragma once

#include <stdexcept>
#include <string>

/**
 * @ingroup logger
 * @brief Базовое исключение системы логирования
 */
class LoggerException : public std::runtime_error {
 public:
  explicit LoggerException(const std::string& message)
      : std::runtime_error("[Logger] " + message) {}
};

/**
 * @ingroup logger
 * @brief Исключение при ошибках инициализации логгера
 */
class LoggerInitException : public LoggerException {
 public:
  explicit LoggerInitException(const std::string& details)
      : LoggerException("Не удалось выполнить инициализацию - " + details) {}
};
