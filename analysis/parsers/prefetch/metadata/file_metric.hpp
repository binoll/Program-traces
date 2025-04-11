/**
 * @file file_metric.hpp
 * @brief Заголовочный файл, содержащий класс для работы с файловыми метриками
 * NTFS
 */

#pragma once

#include <cstdint>
#include <string>

#include "../../../exceptions/prefetch/file_metric_exception.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Проверяет валидность пути к файлу
 * @param[in] path Путь к файлу для проверки
 * @return true если путь не пустой и не состоит только из пробелов, иначе
 * false
 * @note Функция не проверяет существование файла, только синтаксическую
 * корректность пути
 */
[[nodiscard]] static bool is_valid(const std::string& path) noexcept;

/**
 * @class FileMetric
 * @brief Класс-контейнер для метаданных файла NTFS
 *
 * @note Инкапсулирует метаданные файла NTFS, включая путь, MFT-ссылку и
 * дополнительные атрибуты Обеспечивает безопасный доступ к данным через
 * методы-геттеры
 */
class FileMetric final {
 public:
  /// @name Флаги доступа к файлу
  /// @{
  static constexpr uint32_t FILE_METRIC_ACCESS_READ =
      0x01;  ///< Файл был прочитан
  static constexpr uint32_t FILE_METRIC_ACCESS_WRITE =
      0x02;  ///< В файл производилась запись
  static constexpr uint32_t FILE_METRIC_ACCESS_EXECUTE =
      0x04;  ///< Файл был исполнен
  /// @}

  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор с инициализацией всех полей
   * @param[in] filename Полный путь к файлу в формате NTFS
   * @param[in] mft_ref 64-битная ссылка на запись в MFT
   * @param[in] file_size Размер файла в байтах (по умолчанию 0)
   * @param[in] access_flags Битовая маска флагов доступа (по умолчанию 0)
   * @param[in] last_access_time Время последнего доступа в формате FILETIME (по
   * умолчанию 0)
   * @throw InvalidFileMetricException Если путь к файлу некорректен
   * @note Время последнего доступа задается в 100-наносекундных интервалах с
   * 01011601
   */
  FileMetric(std::string filename, uint64_t mft_ref, uint64_t file_size = 0,
             uint32_t access_flags = 0, uint64_t last_access_time = 0);

  /**
   * @brief Деструктор по умолчанию
   */
  ~FileMetric() = default;
  /// @}

  /// @name Базовые методы доступа
  /// @{
  /**
   * @brief Получить путь к файлу
   * @return Константная ссылка на строку с полным путем к файлу
   * @note Путь возвращается в том виде, в котором был передан в конструктор
   */
  [[nodiscard]] const std::string& getFilename() const noexcept;

  /**
   * @brief Получить ссылку на MFT-запись
   * @return 64-битный идентификатор MFT-записи файла
   */
  [[nodiscard]] uint64_t getFileReference() const noexcept;

  /**
   * @brief Получить размер файла
   * @return Размер файла в байтах
   * @note Возвращает 0, если размер неизвестен
   */
  [[nodiscard]] uint64_t getFileSize() const noexcept;

  /**
   * @brief Получить флаги доступа
   * @return Битовая маска флагов доступа
   * @see FILE_METRIC_ACCESS_READ, FILE_METRIC_ACCESS_WRITE,
   * FILE_METRIC_ACCESS_EXECUTE
   */
  [[nodiscard]] uint32_t getAccessFlags() const noexcept;

  /**
   * @brief Получить время последнего доступа
   * @return Время последнего доступа в формате FILETIME
   * @note Формат времени: 100-наносекундные интервалы с 01011601
   */
  [[nodiscard]] uint64_t getLastAccessTime() const noexcept;
  /// @}

  /// @name Методы проверки флагов доступа
  /// @{
  /**
   * @brief Проверить факт чтения файла
   * @return true если установлен флаг FILE_METRIC_ACCESS_READ, иначе false
   */
  [[nodiscard]] bool wasAccessedForRead() const noexcept;

  /**
   * @brief Проверить факт записи в файл
   * @return true если установлен флаг FILE_METRIC_ACCESS_WRITE, иначе false
   */
  [[nodiscard]] bool wasAccessedForWrite() const noexcept;

  /**
   * @brief Проверить факт выполнения файла
   * @return true если установлен флаг FILE_METRIC_ACCESS_EXECUTE, иначе false
   */
  [[nodiscard]] bool wasExecuted() const noexcept;
  /// @}

 private:
  std::string filename_;       ///< Полный NTFS-путь к файлу
  uint64_t file_reference_;    ///< Ссылка на MFT-запись
  uint64_t file_size_;         ///< Размер файла в байтах
  uint32_t access_flags_;      ///< Флаги доступа (битовая маска)
  uint64_t last_access_time_;  ///< Время последнего доступа (FILETIME)
};

}
