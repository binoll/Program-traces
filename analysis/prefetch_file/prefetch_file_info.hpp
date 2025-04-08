/**
 * @file prefetch_file_info.hpp
 * @brief Заголовочный файл с классами для хранения информации из Prefetch-файлов
*/

#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include "exceptions.hpp"
#include "info/file_metric.hpp"
#include "info/volume.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Основной класс для хранения и обработки данных Prefetch
 * @details Содержит метаданные о запусках приложений, включая временные метки,
 *          информацию о томах и связанных файлах
*/
class PrefetchData {
 public:
  /// @name Геттеры основных параметров
  /// @{

  /**
   * @brief Получить имя исполняемого файла
   * @return Строка в формате "ИМЯФАЙЛА.EXE-XХХХХХ"
  */
  [[nodiscard]] std::string executable_name() const noexcept;

  /**
   * @brief Получить хеш Prefetch-файла
   * @return 32-битный хеш, генерируемый Windows
  */
  [[nodiscard]] uint32_t prefetch_hash() const noexcept;

  /**
   * @brief Получить количество запусков приложения
   * @return Число запусков с момента создания Prefetch-файла
  */
  [[nodiscard]] uint32_t run_count() const noexcept;

  /**
   * @brief Получить времена последних запусков
   * @return Вектор временных меток в формате UNIX time
  */
  [[nodiscard]] const std::vector<time_t>& run_times() const noexcept;

  /**
   * @brief Получить информацию о томах
   * @return Вектор объектов VolumeInfo с метаданными разделов
  */
  [[nodiscard]] const std::vector<VolumeInfo>& volumes() const noexcept;

  /**
   * @brief Получить метрики файлов
   * @return Вектор объектов FileMetric с данными о файлах
  */
  [[nodiscard]] const std::vector<FileMetric>& metrics() const noexcept;
  /// @}

  /// @name Сеттеры и модификаторы
  /// @{

  /**
   * @brief Установить имя исполняемого файла
   * @param[in] name Имя файла в формате 8.3 (DOS-формат)
   * @throw InvalidExecutableNameException При:
   * - Пустом имени
   * - Наличии запрещенных символов (\ / : * ? " < > |)
  */
  void set_executable_name(std::string name);

  /**
   * @brief Установить хеш Prefetch-файла
   * @param[in] hash 32-битный хеш, вычисляемый по имени файла и пути
  */
  void set_prefetch_hash(uint32_t hash) noexcept;

  /**
   * @brief Установить количество запусков
   * @param[in] count Число в диапазоне 0-30 (ограничение Windows)
  */
  void set_run_count(uint32_t count) noexcept;

  /**
   * @brief Добавить время запуска
   * @param[in] time UNIX-время последнего запуска приложения
  */
  void add_run_time(time_t time) noexcept;

  /**
   * @brief Добавить информацию о томе
   * @param[in] vol Объект VolumeInfo с метаданными раздела
  */
  void add_volume(VolumeInfo vol);

  /**
   * @brief Добавить метрику файла
   * @param[in] metric Объект FileMetric с данными файла
  */
  void add_metric(FileMetric metric);
  /// @}

 private:
  std::string executable_name_;      ///< Имя в формате "ИМЯ.EXE-ХХХХХХ"
  uint32_t prefetch_hash_ = 0;       ///< Хеш для идентификации Prefetch-файла
  uint32_t run_count_ = 0;           ///< Счетчик запусков (0 ≤ count ≤ 30)
  std::vector<time_t> run_times_;    ///< Времена последних 8 запусков
  std::vector<VolumeInfo> volumes_;  ///< Список связанных логических томов
  std::vector<FileMetric> metrics_;  ///< Список файловых метрик
};

}
