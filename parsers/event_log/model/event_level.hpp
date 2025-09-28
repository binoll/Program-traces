/// @file event_level.hpp
/// @brief Уровни важности событий Windows

#pragma once

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @enum EventLevel
/// @brief Перечисление уровней важности событий Windows
/// @details Соответствует стандартным уровням событий в журналах Windows
enum class EventLevel {
  LogAlways = 0,  ///< Событие всегда записывается в журнал
  Critical = 1,   ///< Критическое событие, серьезный сбой системы
  Error = 2,      ///< Ошибка, требующая внимания администратора
  Warning = 3,    ///< Предупреждение о потенциальной проблеме
  Info = 4,       ///< Информационное сообщение о нормальной работе
  Verbose = 5     ///< Детальная информация для диагностики
};

/// @brief Преобразует уровень события в читаемую строку
/// @param level Уровень события для преобразования
/// @return Строковое представление уровня события
constexpr const char* to_string(EventLevel level) noexcept {
  switch (level) {
    case EventLevel::LogAlways:
      return "Always";
    case EventLevel::Critical:
      return "Critical";
    case EventLevel::Error:
      return "Error";
    case EventLevel::Warning:
      return "Warning";
    case EventLevel::Info:
      return "Info";
    case EventLevel::Verbose:
      return "Verbose";
    default:
      return "Unknown";
  }
}

/// @brief Проверяет, является ли уровень ошибкой
/// @param level Уровень для проверки
/// @return true если уровень Error или Critical
constexpr bool is_error_level(EventLevel level) noexcept {
  return level == EventLevel::Error || level == EventLevel::Critical;
}

/// @brief Проверяет, является ли уровень предупреждением
/// @param level Уровень для проверки
/// @return true если уровень Warning
constexpr bool is_warning_level(EventLevel level) noexcept {
  return level == EventLevel::Warning;
}

/// @brief Проверяет, является ли уровень информационным
/// @param level Уровень для проверки
/// @return true если уровень Info или Verbose
constexpr bool is_info_level(EventLevel level) noexcept {
  return level == EventLevel::Info || level == EventLevel::Verbose;
}

}
