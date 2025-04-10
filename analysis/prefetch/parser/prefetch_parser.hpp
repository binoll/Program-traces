/**
 * @file prefetch_parser.hpp
 * @brief Парсер Prefetch-файлов Windows.
*/

#pragma once

#include <libscca.h>
#include <ctime>
#include <string>

#include "../exceptions/parsing_exception.hpp"
#include "../model/prefetch_data.hpp"
#include "../model/prefetch_data_builder.hpp"
#include "iprefetch_parser.hpp"

namespace PrefetchAnalysis {

/**
 * @class PrefetchParser
 * @brief Класс для чтения и анализа Prefetch-файлов.
*/
class PrefetchParser : public IPrefetchParser {
 public:
  /**
   * @brief Конструктор, инициализирующий библиотеку для работы с Prefetch-файлами.
   * @throw FileOpenException Если не удалось инициализировать библиотеку `libscca`.
  */
  PrefetchParser();

  /**
   * @brief Деструктор, освобождающий ресурсы.
  */
  ~PrefetchParser() noexcept override;

  /**
   * @brief Выполняет парсинг Prefetch-файла по указанному пути.
   * @param path Путь к Prefetch-файлу, который будет разобран.
   * @return Объект `PrefetchDataBuilder`, содержащий распарсенные данные файла.
   * @throw FileOpenException Если файл не найден или не может быть открыт.
   * @throw InvalidFormatException Если файл поврежден или имеет неверный формат.
   * @throw DataReadException Если возникли ошибки при чтении данных из файла.
   * @details Этот метод использует внутренние механизмы для извлечения данных из Prefetch-файла и
   * создает объект, который будет содержать всю необходимую информацию для дальнейшего анализа.
  */
  [[nodiscard]] PrefetchDataBuilder parse(
      const std::string& path) const override;

 private:
  /**
   * @brief Инициализирует внутренние структуры для работы с библиотекой `libscca`.
   * @throw FileOpenException При ошибке инициализации библиотеки `libscca`.
  */
  void initialize();

  /**
   * @brief Парсит основную информацию о приложении из Prefetch-файла.
   * @param data Объект `PrefetchDataBuilder`, в который будет записана распарсенная информация.
   * @throw DataReadException При ошибках чтения данных из файла.
  */
  void parseBasicInfo(PrefetchDataBuilder& data) const;

  /**
   * @brief Парсит историю запусков приложения из Prefetch-файла.
   * @param data Объект `PrefetchDataBuilder`, в который будет записана информация о запуске.
   * @throw InvalidTimestampException При некорректных временных метках.
  */
  void parseRunTimes(PrefetchDataBuilder& data) const;

  /**
   * @brief Парсит информацию о томах из Prefetch-файла.
   * @param data Объект `PrefetchDataBuilder`, в который будет записана информация о томах.
   * @throw InvalidDevicePathException При ошибках в пути устройства.
  */
  void parseVolumes(PrefetchDataBuilder& data) const;

  /**
   * @brief Парсит метрики файлов из Prefetch-файла.
   * @param data Объект `PrefetchDataBuilder`, в который будет записана информация о метриках.
   * @throw InvalidFileMetricException При ошибках в метках файлов.
  */
  void parseMetrics(PrefetchDataBuilder& data) const;

  /**
   * @brief Преобразует значение FILETIME в стандартное UNIX-время.
   * @param filetime Время в формате Windows `FILETIME` (100-наносекундные интервалы с 1601-01-01).
   * @return Время в формате UNIX (с 1970-01-01).
   * @throw InvalidTimestampException Если время в файле недопустимо.
  */
  static time_t convertFiletime(uint64_t filetime);

  libscca_file_t*
      scca_handle_;  ///< Указатель на контекст библиотеки `libscca`.
};

}
