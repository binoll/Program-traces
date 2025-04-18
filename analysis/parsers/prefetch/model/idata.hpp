/**
 * @file idata.hpp
 * @brief Интерфейс для доступа к данным Windows Prefetch-файлов
 */

#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"

namespace PrefetchAnalysis {

/**
 * @class IPrefetchData
 * @brief Интерфейс для чтения и доступа к метаданным Prefetch-файлов
 * @note Определяет методы для получения основных параметров, временных меток,
 * томов и метрик файлов из Prefetch-файла
 */
class IPrefetchData {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Виртуальный деструктор о умолчаниюп
   */
  virtual ~IPrefetchData() noexcept = default;
  /// @}

  /// @name Основные методы доступа
  /// @{
  /**
   * @brief Получить имя исполняемого файла
   * @return Строка в формате "NAME.EXE-XXXXXX"
   */
  [[nodiscard]] virtual std::string getExecutableName() const noexcept = 0;

  /**
   * @brief Получить 32‑битный хеш Prefetch-файла
   * @return Хеш, извлечённый из заголовка файла
   */
  [[nodiscard]] virtual uint32_t getPrefetchHash() const noexcept = 0;

  /**
   * @brief Получить количество запусков приложения
   * @return Число зарегистрированных запусков (≥ 0)
   */
  [[nodiscard]] virtual uint32_t getRunCount() const noexcept = 0;
  /// @}

  /// @name Методы работы с временными метками
  /// @{
  /**
   * @brief Получить список временных меток запусков
   * @return Константная ссылка на вектор временных меток (UNIX time)
   */
  [[nodiscard]] virtual const std::vector<time_t>& getRunTimes()
      const noexcept = 0;

  /**
   * @brief Получить время последнего запуска
   * @return Временная метка последнего запуска (UNIX time)
   */
  [[nodiscard]] virtual time_t getLastRunTime() const noexcept = 0;
  /// @}

  /// @name Методы работы с томами
  /// @{
  /**
   * @brief Получить информацию обо всех томах
   * @return Константная ссылка на вектор объектов VolumeInfo
   */
  [[nodiscard]] virtual const std::vector<VolumeInfo>& getVolumes()
      const noexcept = 0;

  /**
   * @brief Получить основной том, содержащий исполняемый файл
   * @return Объект VolumeInfo основного тома
   * @throw PrefetchDataException Если основной том не найден
   */
  [[nodiscard]] virtual VolumeInfo getMainVolume() const = 0;
  /// @}

  /// @name Методы работы с файловыми метриками
  /// @{
  /**
   * @brief Получить метрики всех файлов, упомянутых в Prefetch-файле
   * @return Константная ссылка на вектор объектов FileMetric
   */
  [[nodiscard]] virtual const std::vector<FileMetric>& getMetrics()
      const noexcept = 0;

  /**
   * @brief Получить метрики только для DLL-файлов
   * @return Вектор объектов FileMetric для DLL-файлов
   */
  [[nodiscard]] virtual std::vector<FileMetric> getDllMetrics() const = 0;
  /// @}

  /// @name Прочие методы
  /// @{
  /**
   * @brief Получить версию формата Prefetch-файла
   * @return Целочисленный идентификатор версии формата (≥ 10)
   */
  [[nodiscard]] virtual uint8_t getFormatVersion() const noexcept = 0;
  /// @}
};

}
