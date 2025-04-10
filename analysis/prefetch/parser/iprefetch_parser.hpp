/**
 * @file iprefetch_parser.hpp
 * @brief Интерфейс для работы с библиотеками, парсящими Prefetch-файлы.
*/

#pragma once

#include <string>
#include "../exceptions/parsing_exception.hpp"

namespace PrefetchAnalysis {
/**
 * @class IPrefetchParser
 * @brief Интерфейс для работы с библиотеками, парсящими Prefetch-файлы.
*/
class IPrefetchParser {
 public:
  /**
   * @brief Виртуальный деструктор интерфейса.
  */
  virtual ~IPrefetchParser() noexcept = default;

  /**
   * @brief Выполняет парсинг указанного Prefetch-файла.
   * @param path Путь к Prefetch-файлу, который должен быть прочитан и разобран.
   * @return Объект `PrefetchDataBuilder`, содержащий все распарсенные данные из файла.
   * @throw FileOpenException Если указанный файл не может быть открыт или недоступен.
   * @throw InvalidFormatException Если структура файла нарушена или файл имеет неверный формат.
   * @throw DataReadException При возникновении ошибок чтения данных из файла.
  */
  [[nodiscard]] virtual PrefetchDataBuilder parse(
      const std::string& path) const = 0;
};

}
