/**
 * @file data.hpp
 * @brief Класс для работы с данными Prefetch-файлов Windows
 */

#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "../../../exceptions/prefetch/prefetch_data_exception.hpp"
#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"
#include "data_storage.hpp"
#include "idata.hpp"
#include "windows_version.hpp"

namespace PrefetchAnalysis {

/**
 * @class PrefetchData
 * @brief Реализация интерфейса IPrefetchData для работы с Prefetch-файлами
 * @details Конкретная реализация интерфейса IPrefetchData, предоставляющая
 * доступ ко всем метаданным Prefetch-файла. Поддерживает все версии Windows
 * от XP до 11
 */
class PrefetchData : public IPrefetchData {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор
   * @param[in] storage  Данные Prefetch-файла в формате перемещаемого объекта
   * @details Конструктор принимает объект хранилища данных Prefetch-файла
   * с использованием семантики перемещения для эффективной инициализации
   */
  explicit PrefetchData(PrefetchDataStorage&& storage);
  /// @}

  /// @name Основные методы доступа
  /// @{
  /// @copydoc IPrefetchData::getExecutableName
  [[nodiscard]] std::string getExecutableName() const noexcept override;

  /// @copydoc IPrefetchData::getPrefetchHash
  [[nodiscard]] uint32_t getPrefetchHash() const noexcept override;

  /// @copydoc IPrefetchData::getRunCount
  [[nodiscard]] uint32_t getRunCount() const noexcept override;
  /// @}

  /// @name Методы работы с временными данными
  /// @{
  /// @copydoc IPrefetchData::getRunTimes
  [[nodiscard]] const std::vector<time_t>& getRunTimes()
      const noexcept override;

  /// @copydoc IPrefetchData::getLastRunTime
  [[nodiscard]] time_t getLastRunTime() const noexcept override;
  /// @}

  /// @name Методы работы с томами
  /// @{
  /// @copydoc IPrefetchData::getVolumes
  [[nodiscard]] const std::vector<VolumeInfo>& getVolumes()
      const noexcept override;

  /// @copydoc IPrefetchData::getMainVolume
  [[nodiscard]] VolumeInfo getMainVolume() const override;
  /// @}

  /// @name Методы работы с файловыми метриками
  /// @{
  /// @copydoc IPrefetchData::getMetrics
  [[nodiscard]] const std::vector<FileMetric>& getMetrics()
      const noexcept override;

  /// @copydoc IPrefetchData::getDllMetrics
  [[nodiscard]] std::vector<FileMetric> getDllMetrics() const override;
  /// @}

  /// @name Методы определения версий
  /// @{
  /// @copydoc IPrefetchData::getWindowsVersion
  [[nodiscard]] WindowsVersion getWindowsVersion() const noexcept override;

  /// @copydoc IPrefetchData::getWindowsVersionString
  [[nodiscard]] std::string getWindowsVersionString() const noexcept override;

  /// @copydoc IPrefetchData::getFormatVersion
  [[nodiscard]] uint8_t getFormatVersion() const noexcept override;
  /// @}

 private:
  PrefetchDataStorage storage_;  ///< Хранилище данных Prefetch-файла
};

}
