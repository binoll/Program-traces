/**
 * @file file_metric.hpp
 * @brief Объявление класса FileMetric для представления метаданных NTFS-файла
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
 * @brief Класс-контейнер для метаданных файла NTFS Prefetch
 * @note Инкапсулирует путь к файлу, ссылку на запись MFT, размер файла,
 * флаги доступа и время последнего доступа. Используется при анализе
 * Prefetch-файлов
 */
class FileMetric final {
 public:
  /// @name Флаги доступа
  /// @{
  static constexpr uint32_t FILE_METRIC_ACCESS_READ =
      0x01;  ///< Файл был прочитан
  static constexpr uint32_t FILE_METRIC_ACCESS_WRITE =
      0x02;  ///< В файл была произведена запись
  static constexpr uint32_t FILE_METRIC_ACCESS_EXECUTE =
      0x04;  ///< Файл был исполнен
  /// @}

  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор, инициализирующий все поля метрики
   * @param[in] filename Полный путь к файлу (в формате NTFS)
   * @param[in] mft_ref Ссылка на запись в таблице MFT
   * @param[in] file_size Размер файла в байтах (по умолчанию 0)
   * @param[in] access_flags Битовая маска флагов доступа (по умолчанию 0)
   * @param[in] last_access_time Время последнего доступа в формате FILETIME (по
   * умолчанию 0)
   * @throw InvalidFileMetricException Если путь некорректен
   * @note Время задаётся в 100-нс интервалах от 01.01.1601 (UTC)
   */
  FileMetric(std::string filename, uint64_t mft_ref, uint64_t file_size = 0,
             uint32_t access_flags = 0, uint64_t last_access_time = 0);

  /**
   * @brief Деструктор по умолчанию
   */
  ~FileMetric() = default;
  /// @}

  /// @name Методы валидации
  /// @{
  /**
   * @brief Проверяет корректность всех метаданных
   * @return true, если путь валиден, MFT > 0 и флаги допустимы
   */
  [[nodiscard]] bool isValid() const noexcept;
  /// @}

  /// @name Геттеры
  /// @{
  /**
   * @brief Получить путь к файлу
   * @return Ссылка на строку с абсолютным путем
   */
  [[nodiscard]] const std::string& getFilename() const noexcept;

  /**
   * @brief Получить ссылку на MFT-запись
   * @return 64-битная ссылка (MFT reference)
   */
  [[nodiscard]] uint64_t getFileReference() const noexcept;

  /**
   * @brief Получить размер файла
   * @return Размер файла в байтах
   */
  [[nodiscard]] uint64_t getFileSize() const noexcept;

  /**
   * @brief Получить флаги доступа
   * @return Битовая маска: READ, WRITE, EXECUTE
   */
  [[nodiscard]] uint32_t getAccessFlags() const noexcept;

  /**
   * @brief Получить время последнего доступа
   * @return FILETIME в 100-нс интервалах от 1601 года
   */
  [[nodiscard]] uint64_t getLastAccessTime() const noexcept;
  /// @}

  /// @name Методы анализа флагов
  /// @{
  /**
   * @brief Проверяет, был ли файл прочитан
   * @return true, если установлен флаг READ
   */
  [[nodiscard]] bool wasAccessedForRead() const noexcept;

  /**
   * @brief Проверяет, была ли запись в файл
   * @return true, если установлен флаг WRITE
   */
  [[nodiscard]] bool wasAccessedForWrite() const noexcept;

  /**
   * @brief Проверяет, был ли файл исполнен
   * @return true, если установлен флаг EXECUTE
   */
  [[nodiscard]] bool wasExecuted() const noexcept;
  /// @}

 private:
  std::string filename_;       ///< Путь к файлу (абсолютный, NTFS)
  uint64_t file_reference_;    ///< Ссылка на запись в MFT
  uint64_t file_size_;         ///< Размер файла в байтах
  uint32_t access_flags_;      ///< Битовая маска: READ, WRITE, EXECUTE
  uint64_t last_access_time_;  ///< FILETIME (100-нс интервалы с 01.01.1601)
};

}
