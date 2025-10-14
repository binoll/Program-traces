/// @file event_data_builder.hpp
/// @brief Строитель для создания объектов EventData

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../exceptions/exceptions.hpp"
#include "event_data.hpp"
#include "event_level.hpp"

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class EventDataBuilder
/// @brief Строитель объектов EventData по паттерну "Строитель"
class EventDataBuilder {
 public:
  /// @brief Конструктор по умолчанию
  EventDataBuilder() = default;

  /// @brief Деструктор по умолчанию
  ~EventDataBuilder() = default;

  /// @brief Устанавливает идентификатор события (lvalue-версия)
  /// @param[in] id Числовой идентификатор события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& event_id(uint32_t id) & noexcept;

  /// @brief Устанавливает идентификатор события (rvalue-версия)
  /// @param[in] id Числовой идентификатор события
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& event_id(uint32_t id) && noexcept;

  /// @brief Устанавливает временную метку события (lvalue-версия)
  /// @param[in] timestamp Временная метка в формате Windows FILETIME
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& timestamp(uint64_t timestamp) & noexcept;

  /// @brief Устанавливает временную метку события (rvalue-версия)
  /// @param[in] timestamp Временная метка в формате Windows FILETIME
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& timestamp(uint64_t timestamp) && noexcept;

  /// @brief Устанавливает уровень важности события (lvalue-версия)
  /// @param[in] level Уровень важности события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& level(EventLevel level) & noexcept;

  /// @brief Устанавливает уровень важности события (rvalue-версия)
  /// @param[in] level Уровень важности события
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& level(EventLevel level) && noexcept;

  /// @brief Устанавливает провайдера события (lvalue-версия)
  /// @param[in] provider Имя провайдера события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& provider(std::string provider) &;

  /// @brief Устанавливает провайдера события (rvalue-версия)
  /// @param[in] provider Имя провайдера события
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& provider(std::string provider) &&;

  /// @brief Устанавливает имя компьютера (lvalue-версия)
  /// @param[in] computer Имя компьютера
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& computer(std::string computer) &;

  /// @brief Устанавливает имя компьютера (rvalue-версия)
  /// @param[in] computer Имя компьютера
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& computer(std::string computer) &&;

  /// @brief Устанавливает канал журнала (lvalue-версия)
  /// @param[in] channel Канал журнала событий
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& channel(std::string channel) &;

  /// @brief Устанавливает канал журнала (rvalue-версия)
  /// @param[in] channel Канал журнала событий
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& channel(std::string channel) &&;

  /// @brief Устанавливает описание события (lvalue-версия)
  /// @param[in] description Текстовое описание события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& description(std::string description) &;

  /// @brief Устанавливает описание события (rvalue-версия)
  /// @param[in] description Текстовое описание события
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& description(std::string description) &&;

  /// @brief Устанавливает XML-представление события (lvalue-версия)
  /// @param[in] xml XML-представление события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& xml(std::string xml) &;

  /// @brief Устанавливает XML-представление события (rvalue-версия)
  /// @param[in] xml XML-представление события
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& xml(std::string xml) &&;

  /// @brief Устанавливает SID пользователя (lvalue-версия)
  /// @param[in] sid SID пользователя
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& user_sid(std::string sid) &;

  /// @brief Устанавливает SID пользователя (rvalue-версия)
  /// @param[in] sid SID пользователя
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& user_sid(std::string sid) &&;

  /// @brief Устанавливает бинарные данные события (lvalue-версия)
  /// @param[in] data Бинарные данные события
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& binary_data(std::vector<uint8_t> data) &;

  /// @brief Устанавливает бинарные данные события (rvalue-версия)
  /// @param[in] data Бинарные данные события
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& binary_data(std::vector<uint8_t> data) &&;

  /// @brief Добавляет дополнительное поле данных (lvalue-версия)
  /// @param[in] key Ключ дополнительного поля
  /// @param[in] value Значение дополнительного поля
  /// @return Ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder& add_data(std::string key, std::string value) &;

  /// @brief Добавляет дополнительное поле данных (rvalue-версия)
  /// @param[in] key Ключ дополнительного поля
  /// @param[in] value Значение дополнительного поля
  /// @return Rvalue-ссылка на текущий построитель для цепочки вызовов
  EventDataBuilder&& add_data(std::string key, std::string value) &&;

  /// @brief Строит итоговый объект EventData
  /// @return Неизменяемый объект EventData
  /// @throws EventBuilderException если обязательные поля не заполнены
  /// @note Метод может быть вызван только на rvalue-объекте
  EventData build() &&;

  /// @brief Проверяет, заполнены ли обязательные поля
  /// @return true если все обязательные поля установлены корректно
  /// @details Обязательные поля: timestamp > 0, provider не пустой, channel не
  /// пустой
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
