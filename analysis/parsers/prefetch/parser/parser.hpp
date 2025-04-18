/**
 * @file parser.hpp
 * @brief Заголовочный файл класса PrefetchParser для анализа Prefetch-файлов
 * Windows.
 */

#pragma once

#include <libscca.h>

#include <ctime>
#include <memory>
#include <string>

#include "../../../exceptions/general/parsing_exception.hpp"
#include "../model/data.hpp"
#include "iparser.hpp"

namespace PrefetchAnalysis {

/// @name Константы для конвертации FILETIME
/// @{
constexpr uint64_t FILETIME_EPOCH_DIFF =
    116444736000000000ULL;  ///< Разница между FILETIME и UNIX-эпохой
constexpr uint64_t FILETIME_MAX_VALID =
    2650467744000000000ULL;  ///< Максимально допустимое значение FILETIME (1
///< янв 2050)
/// @}

/**
 * @class PrefetchParser
 * @brief Основной класс для парсинга Prefetch-файлов с использованием
 * библиотеки libscca
 *
 * @note Реализует интерфейс IPrefetchParser, предоставляя методы для
 * извлечения метаданных, временных меток запуска, информации о томах и метрик
 * файлов из Prefetch-файлов формата Windows
 */
class PrefetchParser : public IPrefetchParser {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор, инициализирующий контекст работы с библиотекой libscca
   * @throw InitLibError При ошибке инициализации библиотеки libscca
   */
  PrefetchParser();

  /**
   * @brief Деструктор, освобождающий ресурсы библиотеки libscca
   * @note Гарантирует безопасное освобождение ресурсов даже в случае
   * исключений
   */
  ~PrefetchParser() noexcept override;
  /// @}

  /// @name Основной интерфейс парсера
  /// @{
  /**
   * @brief Основной метод для парсинга Prefetch-файла
   *
   * @param path Абсолютный путь к анализируемому файлу
   * @return Уникальный указатель на объект с распарсенными данными
   *
   * @throw FileOpenException При проблемах с открытием файла
   * @throw InvalidFormatException При нарушении структуры файла
   * @throw DataReadException При ошибках чтения данных
   *
   * @note Реализует цепочку обработки: открытие файла -> парсинг данных ->
   * закрытие файла
   */
  [[nodiscard]] std::unique_ptr<IPrefetchData> parse(
      const std::string& path) const override;
  /// @}

 private:
  /// @name Вспомогательные методы парсинга
  /// @{
  /**
   * @brief Извлекает базовую информацию о Prefetch-файле
   * @param builder Сборщик данных для заполнения
   * @throw DataReadException При ошибках чтения полей файла
   */
  void parseBasicInfo(PrefetchDataBuilder& builder) const;

  /**
   * @brief Извлекает временные метки запусков приложения
   * @param builder Сборщик данных для заполнения
   * @note Пропускает некорректные временные метки с записью в лог
   */
  void parseRunTimes(PrefetchDataBuilder& builder) const;

  /**
   * @brief Извлекает информацию о связанных томах
   * @param builder Сборщик данных для заполнения
   * @throw DataReadException При ошибках чтения информации о томах
   */
  void parseVolumes(PrefetchDataBuilder& builder) const;

  /**
   * @brief Извлекает метрики файлов из Prefetch-файла
   * @param builder Сборщик данных для заполнения
   * @throw DataReadException При ошибках чтения метрик
   */
  void parseMetrics(PrefetchDataBuilder& builder) const;
  /// @}

  /// @name Утилиты преобразования данных
  /// @{
  /**
   * @brief Конвертирует Windows FILETIME в UNIX-время
   * @param filetime 64-битное значение FILETIME
   * @return Время в формате time_t
   * @throw InvalidTimestampException При некорректном значении FILETIME
   */
  static time_t convertFiletime(uint64_t filetime);
  /// @}

  libscca_file_t* scca_handle_;  ///< Указатель на контекст libscca
};

}
