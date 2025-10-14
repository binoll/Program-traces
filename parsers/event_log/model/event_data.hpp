/// @file event_data.hpp
/// @brief Неизменяемый контейнер данных события Windows

#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "event_level.hpp"

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

class EventDataBuilder;

/// @class EventData
/// @brief Неизменяемый контейнер данных события Windows
class EventData {
 public:
  /// @brief Запрет копирования
  EventData(const EventData&) = delete;

  /// @brief Запрет присваивания копированием
  EventData& operator=(const EventData&) = delete;

  /// @brief Разрешение перемещения
  EventData(EventData&&) = default;

  /// @brief Разрешение присваивания перемещением
  EventData& operator=(EventData&&) = default;

  /// @brief Виртуальный деструктор для корректного наследования
  ~EventData() = default;

  /// @brief Возвращает числовой идентификатор события
  /// @return Числовой идентификатор события (Event ID)
  [[nodiscard]] uint32_t event_id() const noexcept;

  /// @brief Возвращает временную метку в формате Windows FILETIME
  /// @return Временная метка в 100-наносекундных интервалах с 1 января 1601
  /// года
  [[nodiscard]] uint64_t timestamp() const noexcept;

  /// @brief Возвращает уровень важности события
  /// @return Уровень важности события из перечисления EventLevel
  [[nodiscard]] EventLevel level() const noexcept;

  /// @brief Возвращает имя провайдера события
  /// @return Константная ссылка на строку с именем провайдера (источника)
  /// события
  [[nodiscard]] const std::string& provider() const noexcept;

  /// @brief Возвращает имя компьютера
  /// @return Константная ссылка на строку с именем компьютера, где произошло
  /// событие
  [[nodiscard]] const std::string& computer() const noexcept;

  /// @brief Возвращает канал журнала событий
  /// @return Константная ссылка на строку с именем канала журнала событий
  [[nodiscard]] const std::string& channel() const noexcept;

  /// @brief Возвращает текстовое описание события
  /// @return Константная ссылка на строку с текстовым описанием события
  [[nodiscard]] const std::string& description() const noexcept;

  /// @brief Возвращает XML-представление события
  /// @return Константная ссылка на строку с полным XML-представлением события
  [[nodiscard]] const std::string& xml() const noexcept;

  /// @brief Возвращает SID пользователя
  /// @return Константная ссылка на строку с Security Identifier пользователя
  [[nodiscard]] const std::string& user_sid() const noexcept;

  /// @brief Возвращает бинарные данные события
  /// @return Константная ссылка на вектор с бинарными данными события
  [[nodiscard]] const std::vector<uint8_t>& binary_data() const noexcept;

  /// @brief Возвращает словарь дополнительных данных
  /// @return Константная ссылка на словарь с дополнительными параметрами
  /// события
  [[nodiscard]] const std::unordered_map<std::string, std::string>& data()
      const noexcept;

  /// @brief Ищет дополнительное поле данных по ключу
  /// @param[in] key Ключ для поиска в дополнительных данных
  /// @return Optional с значением поля или std::nullopt если поле не найдено
  [[nodiscard]] std::optional<std::string_view> get_data_field(
      std::string_view key) const;

  /// @brief Преобразует Windows FILETIME в стандартную временную точку
  /// @return Временная точка std::chrono::system_clock
  /// @note Возвращает нулевую временную точку при некорректном timestamp
  [[nodiscard]] std::chrono::system_clock::time_point system_timepoint()
      const noexcept;

  /// @brief Проверяет, является ли событие ошибкой или критическим
  /// @return true если уровень события Error или Critical
  [[nodiscard]] bool is_error() const noexcept;

  /// @brief Проверяет, является ли событие предупреждением
  /// @return true если уровень события Warning
  [[nodiscard]] bool is_warning() const noexcept;

  /// @brief Проверяет, является ли событие информационным
  /// @return true если уровень события Info или Verbose
  [[nodiscard]] bool is_info() const noexcept;

  /// @brief Создает построитель для нового события
  /// @return Объект EventDataBuilder для пошагового конструирования события
  [[nodiscard]] static EventDataBuilder builder() noexcept;

 private:
  friend class EventDataBuilder;  ///< Дружественный доступ для построителя

  /// @brief Приватный конструктор, доступный только для EventDataBuilder
  /// @param[in] event_id Числовой идентификатор события
  /// @param[in] timestamp Временная метка в формате Windows FILETIME
  /// @param[in] level Уровень важности события
  /// @param[in] provider Имя провайдера (источника) события
  /// @param[in] computer Имя компьютера, где произошло событие
  /// @param[in] channel Канал журнала событий
  /// @param[in] description Текстовое описание события
  /// @param[in] xml XML-представление события
  /// @param[in] user_sid SID пользователя
  /// @param[in] binary_data Бинарные данные события
  /// @param[in] data Дополнительные параметры события
  EventData(uint32_t event_id, uint64_t timestamp, EventLevel level,
            std::string provider, std::string computer, std::string channel,
            std::string description, std::string xml, std::string user_sid,
            std::vector<uint8_t> binary_data,
            std::unordered_map<std::string, std::string> data);

  uint32_t event_id_;        ///< Числовой идентификатор события
  uint64_t timestamp_;       ///< Временная метка в формате Windows FILETIME
  EventLevel level_;         ///< Уровень важности события
  std::string provider_;     ///< Имя провайдера события
  std::string computer_;     ///< Имя компьютера
  std::string channel_;      ///< Канал журнала событий
  std::string description_;  ///< Текстовое описание события
  std::string xml_;          ///< XML-представление события
  std::string user_sid_;     ///< SID пользователя
  std::vector<uint8_t> binary_data_;  ///< Бинарные данные события
  std::unordered_map<std::string, std::string>
      data_;  ///< Дополнительные параметры
};

}
