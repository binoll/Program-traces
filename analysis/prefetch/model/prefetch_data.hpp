/**
 * @file prefetch_data.hpp
 * @brief Интерфейсы для работы с данными Prefetch-файлов.
*/

#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include "../../exceptions/prefetch/prefetch_data_exception.hpp"
#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"
#include "iprefetch_data.hpp"

namespace PrefetchAnalysis {

/**
 * @class PrefetchData
 * @brief Реализация интерфейса IPrefetchData.
 *
 * @details Хранит все разобранные данные Prefetch-файла и предоставляет доступ к ним через интерфейс IPrefetchData.
*/
class PrefetchData : public IPrefetchData {
 public:
  /**
   * @brief Конструктор PrefetchData.
   * @param name Имя исполняемого файла (включая хеш).
   * @param hash Хеш Prefetch-файла.
   * @param count Количество запусков.
   * @param times Список временных меток запусков.
   * @param vols Список томов.
   * @param mets Список файловых метрик.
  */
  PrefetchData(std::string name, uint32_t hash, uint32_t count,
               std::vector<time_t> times, std::vector<VolumeInfo> vols,
               std::vector<FileMetric> mets);

  /// @copydoc IPrefetchData::getExecutableName
  [[nodiscard]] std::string getExecutableName() const noexcept override;

  /// @copydoc IPrefetchData::getPrefetchHash
  [[nodiscard]] uint32_t getPrefetchHash() const noexcept override;

  /// @copydoc IPrefetchData::getRunCount
  [[nodiscard]] uint32_t getRunCount() const noexcept override;

  /// @copydoc IPrefetchData::getRunTimes
  [[nodiscard]] const std::vector<time_t>& getRunTimes()
      const noexcept override;

  /// @copydoc IPrefetchData::getVolumes
  [[nodiscard]] const std::vector<VolumeInfo>& getVolumes()
      const noexcept override;

  /// @copydoc IPrefetchData::getMetrics
  [[nodiscard]] const std::vector<FileMetric>& getMetrics()
      const noexcept override;

 private:
  std::string
      executable_name_;     ///< Имя исполняемого файла в формате "ИМЯ.EXE-ХЕШ".
  uint32_t prefetch_hash_;  ///< Хеш из заголовка Prefetch-файла.
  uint32_t run_count_;      ///< Количество запусков программы.
  std::vector<time_t>
      run_times_;  ///< Список временных меток запусков (в формате UNIX time).
  std::vector<VolumeInfo>
      volumes_;  ///< Список логических томов, указанных в Prefetch-файле.
  std::vector<FileMetric>
      metrics_;  ///< Метрики доступа к файлам, зарегистрированные в Prefetch.
};

}
