/// @file event_data_builder.hpp
/// @brief Строитель для создания объектов EventData

#pragma once

#include "event_data.hpp"

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class EventDataBuilder
/// @brief Строитель объектов EventData
/// @details Реализует паттерн "Строитель" для пошагового конструирования
/// неизменяемых объектов EventData. Поддерживает цепочки вызовов
class EventDataBuilder {
 public:
  /// @brief Конструктор по умолчанию
  EventDataBuilder() = default;

  /// @brief Деструктор по умолчанию
  ~EventDataBuilder() = default;

  /// @brief Устанавливает идентификатор события
  /// @param id Числовой идентификатор события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& event_id(uint32_t id) & noexcept;
  EventDataBuilder&& event_id(uint32_t id) && noexcept;

  /// @brief Устанавливает временную метку события
  /// @param timestamp Временная метка в формате Windows FILETIME
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& timestamp(uint64_t timestamp) & noexcept;
  EventDataBuilder&& timestamp(uint64_t timestamp) && noexcept;

  /// @brief Устанавливает уровень важности события
  /// @param level Уровень важности события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& level(EventLevel level) & noexcept;
  EventDataBuilder&& level(EventLevel level) && noexcept;

  /// @brief Устанавливает провайдера события
  /// @param provider Имя провайдера события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& provider(std::string provider) &;
  EventDataBuilder&& provider(std::string provider) &&;

  /// @brief Устанавливает имя компьютера
  /// @param computer Имя компьютера
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& computer(std::string computer) &;
  EventDataBuilder&& computer(std::string computer) &&;

  /// @brief Устанавливает канал журнала
  /// @param channel Канал журнала событий
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& channel(std::string channel) &;
  EventDataBuilder&& channel(std::string channel) &&;

  /// @brief Устанавливает описание события
  /// @param description Текстовое описание события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& description(std::string description) &;
  EventDataBuilder&& description(std::string description) &&;

  /// @brief Устанавливает XML-представление события
  /// @param xml XML-представление события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& xml(std::string xml) &;
  EventDataBuilder&& xml(std::string xml) &&;

  /// @brief Устанавливает SID пользователя
  /// @param sid SID пользователя
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& user_sid(std::string sid) &;
  EventDataBuilder&& user_sid(std::string sid) &&;

  /// @brief Устанавливает бинарные данные события
  /// @param data Бинарные данные события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& binary_data(std::vector<uint8_t> data) &;
  EventDataBuilder&& binary_data(std::vector<uint8_t> data) &&;

  /// @brief Добавляет дополнительное поле данных
  /// @param key Ключ дополнительного поля
  /// @param value Значение дополнительного поля
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& add_data(std::string key, std::string value) &;
  EventDataBuilder&& add_data(std::string key, std::string value) &&;

  /// @brief Строит итоговый объект EventData
  /// @return Неизменяемый объект EventData
  /// @throws std::runtime_error если обязательные поля не заполнены
  EventData build() &&;

  /// @brief Проверяет, заполнены ли обязательные поля
  /// @return true если все обязательные поля установлены
  [[nodiscard]] bool is_valid() const noexcept;

 private:
  uint32_t event_id_ = 0;   ///< Числовой идентификатор события
  uint64_t timestamp_ = 0;  ///< Временная метка в формате Windows FILETIME
  EventLevel level_ = EventLevel::LogAlways;  ///< Уровень важности события
  std::string provider_;                      ///< Имя провайдера события
  std::string computer_;                      ///< Имя компьютера
  std::string channel_;                       ///< Канал журнала событий
  std::string description_;                   ///< Текстовое описание события
  std::string xml_;                           ///< XML-представление события
  std::string user_sid_;                      ///< SID пользователя
  std::vector<uint8_t> binary_data_;          ///< Бинарные данные события
  std::unordered_map<std::string, std::string>
      data_;  ///< Дополнительные параметры
};

}
