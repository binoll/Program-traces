/**
* @file PrefetchParser.h
 * @brief Заголовочный файл парсера Prefetch файлов
 */

#pragma once

#include "prefetch_info.hpp"
#include <libscca.h>
#include <string>

/**
 * @brief Класс для работы с Prefetch файлами
 */
class PrefetchParser {
public:
  /**
   * @brief Конструктор парсера
   * @throws std::runtime_error при ошибке инициализации
   */
  PrefetchParser();

  /**
   * @brief Деструктор
   */
  ~PrefetchParser();

  /**
   * @brief Открыть и проанализировать Prefetch файл
   * @param filePath Путь к файлу
   * @return Информация из Prefetch файла
   * @throws std::runtime_error при ошибке парсинга
   */
  PrefetchInfo parse(const std::string& filePath);

private:
  libscca_file_t* sccaFile;
  libscca_error_t* sccaError;

  /**
   * @brief Преобразовать FILETIME в time_t
   * @param filetime Время в формате FILETIME
   * @return Время в формате time_t
   */
  time_t filetimeToTimeT(uint64_t filetime);

  /**
   * @brief Обработать ошибку libscca
   * @param context Контекст ошибки
   * @throws std::runtime_error с описанием ошибки
   */
  void handleError(const std::string& context);
};
