/// @file event_data.hpp
/// @brief Реализует интерфейс IEventData, хранящий все основные атрибуты
/// события Windows

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "ievent_data.hpp"

namespace EventLogAnalysis {

/// @brief Класс, представляющий данные события Windows
/// @details Реализует интерфейс IEventData, хранящий все основные атрибуты
/// события Windows
class EventData : public IEventData {
 public:
  /// @brief Конструктор по умолчанию
  EventData();

  // Setters
  /// @brief Установить идентификатор события
  /// @param[in] id Идентификатор события
  void SetEventID(uint32_t id);

  /// @brief Установить временную метку
  /// @param[in] timestamp Время в формате FILETIME
  void SetTimestamp(uint64_t timestamp);

  /// @brief Установить уровень события
  /// @param[in] level Уровень из перечисления EventLevel
  void SetLevel(EventLevel level);

  /// @brief Установить провайдера события
  /// @param[in] provider Имя провайдера
  void SetProvider(const std::string& provider);

  /// @brief Установить имя компьютера
  /// @param[in] computer Имя компьютера
  void SetComputer(const std::string& computer);

  /// @brief Установить канал журнала
  /// @param[in] channel Имя канала журнала
  void SetChannel(const std::string& channel);

  /// @brief Установить описание события
  /// @param[in] desc Текстовое описание
  void SetDescription(const std::string& desc);

  /// @brief Установить XML-представление
  /// @param[in] xml Строка с XML события
  void SetXml(const std::string& xml);

  /// @brief Добавить параметр события
  /// @param[in] name Имя параметра
  /// @param[in] value Значение параметра
  void AddData(const std::string& name, const std::string& value);

  // Реализация методов интерфейса IEventData
  uint32_t EventID() const override;
  uint64_t Timestamp() const override;
  EventLevel Level() const override;
  const std::string& Provider() const override;
  const std::string& Computer() const override;
  const std::string& Channel() const override;
  const std::string& Description() const override;
  const std::unordered_map<std::string, std::string>& Data() const override;
  const std::string& XmlRepresentation() const override;

 private:
  uint32_t event_id_;   ///< Идентификатор события
  uint64_t timestamp_;  ///< Временная метка в формате FILETIME
  EventLevel level_;    ///< Уровень важности

  std::string provider_;     ///< Провайдер события
  std::string computer_;     ///< Имя компьютера
  std::string channel_;      ///< Канал журнала
  std::string description_;  ///< Описание события
  std::string xml_;          ///< XML-представление

  std::unordered_map<std::string, std::string>
      data_;  ///< Дополнительные данные события
};

}
