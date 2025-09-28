/// @file parser.hpp
/// @brief Парсер для EVTX файлов (Windows Event Log)

#pragma once

#include <libevtx.h>

#include <string>
#include <vector>

#include "../../interfaces/iparser.hpp"
#include "../../model/event_data.hpp"

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class EvtxParser
/// @brief Парсер для EVTX файлов Windows
class EvtxParser final : public IEventLogParser {
 public:
  /// @brief Конструктор
  EvtxParser();

  /// @brief Деструктор
  ~EvtxParser() override;

  /// @brief Конструктор копирования (запрещен)
  EvtxParser(const EvtxParser&) = delete;

  /// @brief Оператор присваивания копированием (запрещен)
  EvtxParser& operator=(const EvtxParser&) = delete;

  /// @brief Парсит все события из EVTX файла
  /// @param file_path Путь к EVTX файлу
  /// @return Вектор разобранных событий
  std::vector<EventData> parse(const std::string& file_path) override;

  /// @brief Фильтрует события по идентификатору из EVTX файла
  /// @param file_path Путь к EVTX файлу
  /// @param event_id Идентификатор события для фильтрации
  /// @return Вектор событий с указанным идентификатором
  std::vector<EventData> filterByEventId(const std::string& file_path,
                                         uint32_t event_id) override;

  /// @brief Проверяет поддержку формата файла
  /// @param file_path Путь к файлу для проверки
  /// @return true если файл имеет расширение .evtx
  [[nodiscard]] bool supports(const std::string& file_path) const override;

  /// @brief Возвращает название формата
  /// @return Строка с названием формата "Windows Event Log (EVTX)"
  [[nodiscard]] std::string formatName() const override;

  /// @brief Возвращает поддерживаемые расширения файлов
  /// @return Вектор расширений {".evtx", ".evt-x"}
  [[nodiscard]] std::vector<std::string> supportedExtensions() const override;

 private:
  /// @brief Открывает EVTX файл для чтения
  /// @param file_path Путь к EVTX файлу
  /// @throws std::runtime_error Если не удалось открыть файл
  void openFile(const std::string& file_path);

  /// @brief Закрывает открытый EVTX файл
  void closeFile() noexcept;

  /// @brief Парсит одну запись из EVTX файла
  /// @param record Указатель на запись libevtx
  /// @return Объект EventData с разобранными данными
  [[nodiscard]] static EventData parseRecord(libevtx_record_t* record);

  /// @brief Обрабатывает ошибку библиотеки libevtx
  /// @param context Контекст ошибки для сообщения
  /// @param error Указатель на объект ошибки libevtx
  /// @throws std::runtime_error С подробным сообщением об ошибке
  static void handleLibEvtxError(const std::string& context,
                                 libevtx_error_t* error);

  libevtx_file_t* evtx_file_ =
      nullptr;                ///< Указатель на файловый объект libevtx
  bool file_opened_ = false;  ///< Флаг открытого состояния файла
};

}
