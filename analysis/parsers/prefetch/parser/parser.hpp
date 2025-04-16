/**
 * @file parser.hpp
 * @brief Класс PrefetchParser — основной компонент для анализа Prefetch-файлов
 * Windows
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

/**
 * @class PrefetchParser
 * @brief Реализация интерфейса IPrefetchParser для анализа Prefetch-файлов.
 *
 * Обрабатывает файл по заданному пути, извлекает все доступные метаданные и
 * возвращает готовый к использованию объект PrefetchData.
 */
class PrefetchParser : public IPrefetchParser {
 public:
  /**
   * @brief Конструктор класса.
   *
   * Инициализирует внутренние структуры библиотеки `libscca`.
   * @throw InitLibError если инициализация libscca завершилась неудачей.
   */
  PrefetchParser();

  /**
   * @brief Деструктор класса.
   *
   * Освобождает все ресурсы, связанные с libscca.
   */
  ~PrefetchParser() noexcept override;

  /**
   * @brief Парсит указанный Prefetch-файл.
   *
   * @param path Полный путь к Prefetch-файлу.
   * @return PrefetchData Объект, содержащий все извлеченные данные.
   *
   * @throw FileOpenException Если файл не удалось открыть.
   * @throw InvalidFormatException Если структура файла некорректна.
   * @throw DataReadException Если произошла ошибка чтения данных.
   */
  [[nodiscard]] std::unique_ptr<IPrefetchData> parse(
      const std::string& path) const override;

 private:
  /**
   * @brief Чтение основной информации: имя файла, хеш, счетчик запусков.
   */
  void parseBasicInfo(PrefetchDataBuilder& builder) const;

  /**
   * @brief Извлечение временных меток запусков.
   */
  void parseRunTimes(PrefetchDataBuilder& builder) const;

  /**
   * @brief Извлечение информации о томах.
   */
  void parseVolumes(PrefetchDataBuilder& builder) const;

  /**
   * @brief Извлечение метрик файлов.
   */
  void parseMetrics(PrefetchDataBuilder& builder) const;

  /**
   * @brief Преобразование Windows FILETIME в UNIX time.
   *
   * @param filetime Значение времени в формате FILETIME.
   * @return Время в формате `time_t`.
   * @throw InvalidTimestampException Если метка времени выходит за допустимые
   * границы.
   */
  static time_t convertFiletime(uint64_t filetime);

  /**
   * @brief Указатель на внутренний контекст библиотеки libscca.
   */
  libscca_file_t* scca_handle_;
};

}
