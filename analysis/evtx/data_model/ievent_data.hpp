/// @file ievent_data.hpp
/// @brief Интерфейс для данных события Windows

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace EventLogAnalysis {

/// @brief Уровни событий Windows
enum class EventLevel {
  LOG_ALWAYS = 0,  ///< Всегда записывается в журнал
  CRITICAL = 1,    ///< Критическое событие
  ERROR = 2,       ///< Ошибка
  WARNING = 3,     ///< Предупреждение
  INFO = 4,        ///< Информационное сообщение
  VERBOSE = 5      ///< Детальная информация
};

/// @class IEventData
/// @brief Интерфейс для данных события Windows
class IEventData {
 public:
  /// @brief Виртуальный деструктор
  virtual ~IEventData() = default;

  /// @brief Получить идентификатор события
  /// @return Числовой идентификатор события
  [[nodiscard]] virtual uint32_t EventID() const = 0;

  /// @brief Получить временную метку события
  /// @return Время в формате Windows FILETIME (100-нс интервалы с 01.01.1601)
  [[nodiscard]] virtual uint64_t Timestamp() const = 0;

  /// @brief Получить уровень важности события
  /// @return Уровень события из перечисления EventLevel
  [[nodiscard]] virtual EventLevel Level() const = 0;

  /// @brief Получить провайдера события
  /// @return Имя провайдера (источника события)
  [[nodiscard]] virtual const std::string& Provider() const = 0;

  /// @brief Получить имя компьютера
  /// @return Имя компьютера, где произошло событие
  [[nodiscard]] virtual const std::string& Computer() const = 0;

  /// @brief Получить канал журнала
  /// @return Имя канала журнала событий
  [[nodiscard]] virtual const std::string& Channel() const = 0;

  /// @brief Получить описание события
  /// @return Текстовое описание события
  [[nodiscard]] virtual const std::string& Description() const = 0;

  /// @brief Получить дополнительные данные события
  /// @return Словарь с дополнительными параметрами события
  [[nodiscard]] virtual const std::unordered_map<std::string, std::string>& Data() const = 0;

  /// @brief Получить XML-представление события
  /// @return Строка с полным XML события
  [[nodiscard]] virtual const std::string& XmlRepresentation() const = 0;
};

}
