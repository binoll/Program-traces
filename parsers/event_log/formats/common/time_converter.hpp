/// @file time_converter.hpp
/// @brief Утилиты для конвертации временных форматов Windows

#pragma once

#include <chrono>

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class TimeConverter
/// @brief Конвертер временных форматов Windows
class TimeConverter {
 public:
  /// @brief Конструктор по умолчанию
  TimeConverter() = default;

  /// @brief Деструктор по умолчанию
  ~TimeConverter() = default;

  /// @brief Конвертирует Windows FILETIME в system_clock::time_point
  /// @param filetime Время в формате Windows FILETIME (100-нс интервалы с 1601
  /// г.)
  /// @return Временная точка std::chrono::system_clock::time_point
  static std::chrono::system_clock::time_point filetimeToSystemTime(
      uint64_t filetime);

  /// @brief Конвертирует Unix timestamp в Windows FILETIME
  /// @param unix_time Время в формате Unix timestamp (секунды с 1970 г.)
  /// @return Время в формате Windows FILETIME
  static uint64_t unixTimeToFiletime(time_t unix_time);

  /// @brief Конвертирует system_clock::time_point в Windows FILETIME
  /// @param tp Временная точка std::chrono::system_clock::time_point
  /// @return Время в формате Windows FILETIME
  static uint64_t systemTimeToFiletime(
      const std::chrono::system_clock::time_point& tp);

  /// @brief Конвертирует секунды с 1970 в FILETIME
  /// @param seconds Количество секунд с 1 января 1970 года
  /// @return Время в формате Windows FILETIME
  static uint64_t secondsSince1970ToFiletime(uint64_t seconds);

 private:
  static constexpr uint64_t FILETIME_EPOCH_DIFFERENCE =
      116444736000000000ULL;  ///< Разница между эпохами Windows FILETIME (1601)
                              ///< и Unix (1970) в 100-нс интервалах
  static constexpr uint64_t HUNDRED_NS_PER_SECOND =
      10000000ULL;  ///< Количество 100-наносекундных интервалов в одной секунде
  static constexpr uint64_t NS_PER_FILETIME_TICK =
      100ULL;  ///< Количество наносекунд в одном интервале FILETIME
};

}
