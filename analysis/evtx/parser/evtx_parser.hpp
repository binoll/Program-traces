#pragma once

#include <libevtx.h>

#include <memory>
#include <string>
#include <vector>

#include "../../../core/exceptions/parsing_exception.hpp"
#include "../../../utils/logging/logger.hpp"
#include "../data_model/event_data.hpp"
#include "ievent_log_parser.hpp"

namespace EventLogAnalysis {

/// @class EvtxParser
/// @brief Парсер событий из EVTX файлов с использованием библиотеки libevtx
class EvtxParser final : public IEventLogParser {
 public:
  /// @brief Конструктор по умолчанию
  /// @throws InitLibError При ошибке инициализации библиотеки libevtx
  EvtxParser();

  /// @brief Деструктор
  ~EvtxParser() override;

  EvtxParser(const EvtxParser&) = delete;
  EvtxParser& operator=(const EvtxParser&) = delete;

  /// @brief Распарсить все события из файла
  /// @param[in] file_path Путь к EVTX файлу
  /// @return Вектор распарсенных событий
  /// @throws ParsingException При ошибке открытия файла или чтения данных
  std::vector<std::unique_ptr<IEventData>> ParseEvents(
      const std::string& file_path) override;

  /// @brief Получить события по конкретному идентификатору
  /// @param[in] file_path Путь к EVTX файлу
  /// @param[in] event_id Идентификатор события для фильтрации
  /// @return Вектор отфильтрованных событий
  /// @throws ParsingException При ошибке открытия файла или чтения данных
  std::vector<std::unique_ptr<IEventData>> GetEventsByType(
      const std::string& file_path, uint32_t event_id) override;

 private:
  /// @brief Распарсить запись события из libevtx
  /// @param[in] record Указатель на запись libevtx_record_t
  /// @return Уникальный указатель на распарсенные данные события
  static std::unique_ptr<EventData> ParseRecord(libevtx_record_t* record);

  /// @brief Извлечь дополнительные данные события из XML-представления
  /// @param[out] event_data Объект для заполнения данными
  /// @param[in] xml XML-строка для парсинга
  static void ExtractEventDataFromXml(EventData& event_data,
                                      const std::string& xml);

  /// @brief Открыть EVTX файл для парсинга
  /// @param[in] file_path Путь к EVTX файлу
  /// @throws FileOpenException При ошибке открытия файла
  void OpenLogFile(const std::string& file_path);

  /// @brief Закрыть текущий открытый EVTX файл
  void CloseLogFile();

  libevtx_file_t* evtx_file_ = nullptr;  ///< Указатель на объект файла libevtx
  bool file_opened_ = false;             ///< Флаг открытого файла
};

}
