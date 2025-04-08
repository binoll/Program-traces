/**
 * @file prefetch_file_parser.hpp
 * @brief Парсер Prefetch-файлов Windows
*/

#pragma once

#include <libscca.h>
#include <ctime>
#include <memory>
#include <string>
#include "exceptions.hpp"
#include "prefetch_file_info.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Класс для чтения и анализа Prefetch-файлов
 * @details Реализует парсинг файлов в формате Windows Prefetch (PF)
 *          с преобразованием данных во внутреннее представление PrefetchData
 */
class PrefetchParser {
 public:
  /**
     * @brief Конструктор инициализирует парсер
     * @throw FileOpenException Если не удалось инициализировать библиотеку libscca
     */
  PrefetchParser();

  /**
     * @brief Деструктор освобождает ресурсы
     */
  ~PrefetchParser() noexcept;

  /**
     * @brief Выполняет парсинг указанного файла
     * @param path Путь к Prefetch-файлу
     * @return Объект PrefetchData с распарсенными данными
     * @throw FileOpenException Если файл не найден или недоступен
     * @throw InvalidFormatException При нарушении структуры файла
     * @throw DataReadException При ошибках чтения данных
     */
  [[nodiscard]] PrefetchData parse(const std::string& path) const;

 private:
  /**
     * @brief Инициализирует внутренние структуры парсера
     * @throw FileOpenException При ошибках инициализации
     */
  void initialize();

  /**
     * @brief Парсинг основной информации о приложении
     * @param data Объект для сохранения результатов
     * @throw DataReadException При ошибках чтения данных
     */
  void parse_basic_info(PrefetchData& data) const;

  /**
     * @brief Парсинг истории запусков приложения
     * @param data Объект для сохранения результатов
     * @throw InvalidTimestampException При недопустимых временных метках
     */
  void parse_run_times(PrefetchData& data) const;

  /**
     * @brief Парсинг информации о томах
     * @param data Объект для сохранения результатов
     * @throw InvalidDevicePathException При некорректных путях устройств
     */
  void parse_volumes(PrefetchData& data) const;

  /**
     * @brief Парсинг файловых метрик
     * @param data Объект для сохранения результатов
     * @throw InvalidFileMetricException При некорректных параметрах файлов
     */
  void parse_metrics(PrefetchData& data) const;

  /**
     * @brief Конвертирует FILETIME в UNIX-время
     * @param filetime Время в 100-нс интервалах с 1601-01-01
     * @return Время в секундах с 1970-01-01
     * @throw InvalidTimestampException При недопустимом значении времени
     */
  [[nodiscard]] static time_t convert_filetime(uint64_t filetime);

  libscca_file_t* scca_handle_;  ///< Указатель на контекст библиотеки libscca
};

}
