/// @file parser.hpp
/// @brief Парсер для EVT файлов (Windows Event Log)

#pragma once

#include <libevt.h>

#include <string>
#include <vector>

#include "../../../event_log/interfaces/iparser.hpp"
#include "../../../event_log/model/event_data.hpp"

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class EvtParser
/// @brief Парсер для EVT файлов Windows
class EvtParser final : public IEventLogParser {
 public:
  /// @brief Коснтурктор
  EvtParser();

  /// @brief Деструктор
  ~EvtParser() override;

  /// @brief Конструктор копирования
  EvtParser(const EvtParser&) = delete;

  /// @brief Оператор присваивания копированием
  EvtParser& operator=(const EvtParser&) = delete;

  /// @brief Парсит все события из EVT файла
  /// @param file_path Путь к EVT файлу
  /// @return Вектор разобранных событий
  std::vector<EventData> parse(const std::string& file_path) override;

  /// @brief Фильтрует события по идентификатору из EVT файла
  /// @param file_path Путь к EVT файлу
  /// @param event_id Идентификатор события для фильтрации
  /// @return Вектор событий с указанным идентификатором
  std::vector<EventData> filterByEventId(const std::string& file_path,
                                         uint32_t event_id) override;

  /// @brief Проверяет поддержку формата файла
  /// @param file_path Путь к файлу для проверки
  /// @return true если файл имеет расширение .evt
  [[nodiscard]] bool supports(const std::string& file_path) const override;

  /// @brief Возвращает название формата
  /// @return Строка с названием формата "Windows Event Log (EVT)"
  [[nodiscard]] std::string formatName() const override;

  /// @brief Возвращает поддерживаемые расширения файлов
  /// @return Вектор расширений {".evt"}
  [[nodiscard]] std::vector<std::string> supportedExtensions() const override;

 private:
  /// @brief Открывает EVT файл для чтения
  /// @param file_path Путь к EVT файлу
  /// @throws std::runtime_error Если не удалось открыть файл
  void openFile(const std::string& file_path);

  /// @brief Закрывает открытый EVT файл
  void closeFile() noexcept;

  /// @brief Парсит одну запись из EVT файла
  /// @param record Указатель на запись libevt
  /// @return Объект EventData с разобранными данными
  static EventData parseRecord(libevt_record_t* record);

  /// @brief Конвертирует тип события EVT в уровень важности
  /// @param event_type Тип события из libevt
  /// @return Уровень важности EventLevel
  [[nodiscard]] static EventLevel convertEventType(
      uint16_t event_type) noexcept;

  /// @brief Обрабатывает ошибку библиотеки libevt
  /// @param context Контекст ошибки для сообщения
  /// @param error Указатель на объект ошибки libevt
  /// @throws std::runtime_error С подробным сообщением об ошибке
  static void handleLibEvtError(const std::string& context,
                                libevt_error_t* error);

  libevt_file_t* evt_file_ = nullptr;  ///< Указатель на файловый объект libevt
  bool file_opened_ = false;           ///< Флаг открытого состояния файла
};

}
