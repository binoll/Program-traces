/// @file exceptions.hpp
/// @brief Кастомные исключения для работы с событиями Windows

#pragma once

#include <stdexcept>
#include <string>

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @brief Базовое исключение для ошибок, связанных с событиями Windows
class EventException : public std::runtime_error {
 public:
  /// @brief Конструктор исключения
  /// @param message Сообщение об ошибке
  explicit EventException(const std::string& message)
      : std::runtime_error(message) {}
};

/// @brief Исключение для ошибок в данных события
class InvalidEventDataException : public EventException {
 public:
  /// @brief Конструктор исключения
  /// @param message Сообщение об ошибке в данных события
  explicit InvalidEventDataException(const std::string& message)
      : EventException(message) {}
};

/// @brief Исключение для ошибок построителя событий
class EventBuilderException : public EventException {
 public:
  /// @brief Конструктор исключения
  /// @param message Сообщение об ошибке построителя
  explicit EventBuilderException(const std::string& message)
      : EventException(message) {}
};

}
