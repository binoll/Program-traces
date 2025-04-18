/**
 * @file data_builder.hpp
 * @brief Интерфейс для построения объектов PrefetchData с валидацией и сборкой
 * данных
 */

#pragma once

#include <ctime>
#include <memory>
#include <string>

#include "../../../exceptions/prefetch/prefetch_data_exception.hpp"
#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"
#include "data_storage.hpp"
#include "idata.hpp"

namespace PrefetchAnalysis {

/**
 * @class PrefetchDataBuilder
 * @brief Пошаговый строитель объектов PrefetchData с валидацией параметров
 * @details Реализует паттерн "Строитель" для создания объектов данных
 * Prefetch-файлов. Обеспечивает проверку обязательных параметров и контроль
 * целостности данных
 */
class PrefetchDataBuilder {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор по умолчанию
   */
  PrefetchDataBuilder() = default;
  /// @}

  /// @name Основные методы настройки параметров
  /// @{
  /**
   * @brief Устанавливает имя исполняемого файла
   * @param name Имя в формате "NAME.EXE-XXXXXX"
   * @return Ссылка на текущий объект строителя
   * @throw PrefetchDataException Если имя пустое или не соответствует формату
   */
  PrefetchDataBuilder& setExecutableName(std::string name);

  /**
   * @brief Устанавливает хеш Prefetch-файла
   * @param hash 32-битный хеш файла
   * @return Ссылка на текущий объект строителя
   */
  PrefetchDataBuilder& setPrefetchHash(uint32_t hash) noexcept;

  /**
   * @brief Устанавливает количество запусков приложения
   * @param count Неотрицательное целое число
   * @return Ссылка на текущий объект строителя
   */
  PrefetchDataBuilder& setRunCount(uint32_t count) noexcept;

  /**
   * @brief Устанавливает версию формата Prefetch-файла
   * @param format_version Номер версии (≥10)
   * @return Ссылка на текущий объект строителя
   */
  PrefetchDataBuilder& setFormatVersion(uint8_t format_version) noexcept;

  /**
   * @brief Устанавливает время последнего запуска
   * @param last_run_time Время в формате UNIX
   * @return Ссылка на текущий объект строителя
   */
  PrefetchDataBuilder& setLastRunTime(time_t last_run_time) noexcept;
  /// @}

  /// @name Методы добавления элементов коллекций
  /// @{
  /**
   * @brief Добавляет временную метку запуска
   * @param time UNIX-время запуска приложения
   * @return Ссылка на текущий объект строителя
   */
  PrefetchDataBuilder& addRunTime(time_t time) noexcept;

  /**
   * @brief Добавляет информацию о томе
   * @param vol Объект VolumeInfo
   * @return Ссылка на текущий объект строителя
   * @throw PrefetchDataException При невалидных данных тома
   */
  PrefetchDataBuilder& addVolume(const VolumeInfo& vol);

  /**
   * @brief Добавляет файловую метрику
   * @param metric Объект FileMetric
   * @return Ссылка на текущий объект строителя
   * @throw PrefetchDataException При невалидных данных метрики
   */
  PrefetchDataBuilder& addMetric(const FileMetric& metric);
  /// @}

  /// @name Методы завершения сборки
  /// @{
  /**
   * @brief Завершает процесс сборки и возвращает готовый объект
   * @return Указатель на интерфейс IPrefetchData
   * @throw PrefetchDataException При:
   * - Отсутствии имени исполняемого файла
   * - Некорректной версии формата
   * - Несоответствии количества запусков и временных меток
   */
  [[nodiscard]] std::unique_ptr<IPrefetchData> build();
  /// @}

 private:
  PrefetchDataStorage storage_;  ///< Хранилище данных Prefetch-файла
};

}
