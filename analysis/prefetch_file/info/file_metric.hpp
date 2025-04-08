/**
 * @file file_metric.hpp
 * @brief Заголовочный файл класса FileMetric для хранения метрик файла
*/

#pragma once

#include <cstdint>
#include <string>
#include "../exceptions.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Класс для хранения идентификационной информации файла
 * @details Содержит полное имя файла и его MFT-ссылку для работы с NTFS
*/
class FileMetric {
 public:
  /**
   * @brief Конструктор с инициализацией основных параметров
   * @param[in] name Полное имя файла в формате NTFS
   * @param[in] ref 64-битная MFT-ссылка файла
   *
   * @throw FileMetricException Если имя файла невалидно
  */
  FileMetric(std::string name, uint64_t ref);

  /// @name Геттеры основных атрибутов
  /// @{
  /**
   * @brief Получить полное имя файла
   * @return Строка с абсолютным путем к файлу в NTFS-формате
  */
  [[nodiscard]] std::string filename() const noexcept;

  /**
   * @brief Получить MFT-идентификатор файла
   * @return 64-битная ссылка на запись в главной файловой таблице
  */
  [[nodiscard]] uint64_t file_reference() const noexcept;
  /// @}

 private:
  std::string filename_;     ///< Полное имя файла в системном формате
  uint64_t file_reference_;  ///< Идентификатор MFT-записи файла
};

}
