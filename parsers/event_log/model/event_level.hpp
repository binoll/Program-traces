/// @file event_level.hpp
/// @brief Уровни важности событий Windows

#pragma once

#include <string_view>

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @enum EventLevel
/// @brief Перечисление уровней важности событий Windows
enum class EventLevel {
  LogAlways = 0,  ///< Событие всегда записывается в журнал независимо от уровня
                  ///< фильтрации
  Critical = 1,  ///< Критическое событие, указывающее на серьезный сбой системы
  Error = 2,     ///< Ошибка, требующая внимания администратора
  Warning = 3,   ///< Предупреждение о потенциальной проблеме
  Info = 4,      ///< Информационное сообщение о нормальной работе системы
  Verbose = 5    ///< Детальная информация для диагностики и отладки
};

/// @brief Преобразует уровень события в читаемую строку
/// @param[in] level Уровень события для преобразования
/// @return Строковое представление уровня события
/// @note Возвращает "Unknown" для неизвестных уровней
constexpr std::string_view to_string(EventLevel level) noexcept {
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
/// @param[in] level Уровень события для проверки
/// @return true если уровень Error или Critical
/// @return false для других уровней
constexpr bool is_error_level(EventLevel level) noexcept {
  return level == EventLevel::Error || level == EventLevel::Critical;
}

/// @brief Проверяет, является ли уровень предупреждением
/// @param[in] level Уровень события для проверки
/// @return true если уровень Warning
/// @return false для других уровней
constexpr bool is_warning_level(EventLevel level) noexcept {
  return level == EventLevel::Warning;
}

/// @brief Проверяет, является ли уровень информационным
/// @param[in] level Уровень события для проверки
/// @return true если уровень Info или Verbose
/// @return false для других уровней
constexpr bool is_info_level(EventLevel level) noexcept {
  return level == EventLevel::Info || level == EventLevel::Verbose;
}

}
