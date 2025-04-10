/**
 * @file file_metric.hpp
 * @brief Классы для работы с файловыми метриками NTFS.
*/

#pragma once

#include <cstdint>
#include <string>
#include "../../exceptions/prefetch/file_metric_exception.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Проверяет валидность пути к файлу.
 * @details Функция проверяет, что путь не пустой и не состоит только из пробелов.
 * @param path Путь к файлу, который нужно проверить.
 * @return true, если путь корректен, иначе false.
*/
[[nodiscard]] static bool is_valid(const std::string& path) noexcept;

/**
 * @class FileMetric
 * @brief Контейнер для метаданных файла NTFS.
 * @details Класс инкапсулирует метаданные файла NTFS, включая путь к файлу и ссылку на его запись в MFT.
 *          Обеспечивает методы для получения пути и MFT-идентификатора.
*/
class FileMetric final {
 public:
  /**
   * @brief Конструктор с обязательной валидацией пути.
   * @details Инициализирует объект с данными о файле, проверяя валидность пути.
   * @param filename Полный путь к файлу.
   * @param mft_ref 64-битная ссылка на запись в MFT.
   * @throw InvalidFileMetricException Исключение выбрасывается, если путь к файлу некорректен.
  */
  FileMetric(std::string filename, uint64_t mft_ref);

  /**
   * @brief Получить путь к файлу.
   * @details Метод возвращает путь к файлу, сохраненный при инициализации объекта.
   * @return Константная ссылка на строку с полным путем к файлу.
  */
  [[nodiscard]] const std::string& getFilename() const noexcept;

  /**
   * @brief Получить MFT-идентификатор.
   * @details Метод возвращает 64-битный идентификатор MFT-записи, связанный с данным файлом.
   * @return 64-битная ссылка на запись MFT файла.
  */
  [[nodiscard]] uint64_t getFileReference() const noexcept;

 private:
  const std::string filename_;     ///< Полный путь к файлу в формате NTFS.
  const uint64_t file_reference_;  ///< 64-битная ссылка на запись в MFT.
};

}
