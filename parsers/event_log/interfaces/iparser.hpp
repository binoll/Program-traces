/// @file iparser.hpp
/// @brief Интерфейс парсера журналов событий Windows

#pragma once

#include <string>
#include <vector>

#include "../model/event_data.hpp"

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class IEventLogParser
/// @brief Базовый интерфейс парсера журналов событий
class IEventLogParser {
 public:
  /// @brief Виртуальный деструктор по умолчанию
  virtual ~IEventLogParser() = default;

  /// @brief Парсит все события из файла журнала
  /// @param file_path Путь к файлу журнала событий
  /// @return Вектор объектов EventData с разобранными событиями
  virtual std::vector<EventData> parse(const std::string& file_path) = 0;

  /// @brief Фильтрует события по идентификатору
  /// @param file_path Путь к файлу журнала событий
  /// @param event_id Идентификатор события для фильтрации
  /// @return Вектор объектов EventData, отфильтрованных по event_id
  virtual std::vector<EventData> filterByEventId(const std::string& file_path,
                                                 uint32_t event_id) = 0;

  /// @brief Проверяет поддержку формата файла
  /// @param file_path Путь к файлу для проверки
  /// @return true если формат файла поддерживается парсером
  [[nodiscard]] virtual bool supports(const std::string& file_path) const = 0;

  /// @brief Возвращает описание формата
  /// @return Строка с человеко-читаемым описанием формата
  [[nodiscard]] virtual std::string formatName() const = 0;

  /// @brief Возвращает расширения файлов формата
  /// @return Вектор строк с поддерживаемыми расширениями файлов
  [[nodiscard]] virtual std::vector<std::string> supportedExtensions()
      const = 0;
};

}
