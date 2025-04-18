#pragma once

#include <string>

#include "../../../exceptions/general/parsing_exception.hpp"
#include "../model/data_builder.hpp"

namespace PrefetchAnalysis {

/**
 * @class IPrefetchParser
 * @brief Интерфейс для парсинга Prefetch-файлов
 * @details Данный интерфейс определяет контракт для всех классов, реализующих
 * функциональность по чтению и разбору Prefetch-файлов
 */
class IPrefetchParser {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Виртуальный деструктор по умолчанию
   */
  virtual ~IPrefetchParser() noexcept = default;
  /// @}

  ///@name Методы парсинга
  /// @{
  /**
   * @brief Выполняет парсинг указанного Prefetch-файла
   * @param path Полный путь к Prefetch-файлу
   * @return Объект `PrefetchData`, содержащий распарсенные данные
   * @throw FileOpenException Если файл не может быть открыт или не существует
   * @throw InvalidFormatException Если файл имеет некорректную структуру или не
   * соответствует ожидаемому формату
   * @throw DataReadException Если произошла ошибка при чтении содержимого
   * файла
   */
  [[nodiscard]] virtual std::unique_ptr<IPrefetchData> parse(
      const std::string& path) const = 0;
  /// @}
};

}
