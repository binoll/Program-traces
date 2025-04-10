/**
 * @file iprefetch_data.hpp
 * @brief Интерфейсы для работы с данными Prefetch-файлов.
*/

#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"

namespace PrefetchAnalysis {
/**
 * @class IPrefetchData
 * @brief Интерфейс неизменяемых данных Prefetch-файла.
 *
 * @details Предоставляет абстрактный доступ к ключевым данным Prefetch-файла,
 * включая имя исполняемого файла, хеш, количество запусков, временные метки,
 * информацию о томах и метрики файлов.
*/
class IPrefetchData {
 public:
  /**
   * @brief Виртуальный деструктор.
  */
  virtual ~IPrefetchData() = default;

  /**
   * @brief Получить имя исполняемого файла.
   * @return Строка в формате "ИМЯ.EXE-ХЕШ".
  */
  [[nodiscard]] virtual std::string getExecutableName() const noexcept = 0;

  /**
   * @brief Получить 32-битный хеш из заголовка Prefetch-файла.
   * @return Значение хеша.
  */
  [[nodiscard]] virtual uint32_t getPrefetchHash() const noexcept = 0;

  /**
   * @brief Получить количество запусков программы.
   * @return Число.
  */
  [[nodiscard]] virtual uint32_t getRunCount() const noexcept = 0;

  /**
   * @brief Получить список временных меток запусков.
   * @return Ссылка на вектор меток времени (UNIX-время).
  */
  [[nodiscard]] virtual const std::vector<time_t>& getRunTimes()
      const noexcept = 0;

  /**
   * @brief Получить список обнаруженных томов.
   * @return Ссылка на вектор объектов VolumeInfo.
  */
  [[nodiscard]] virtual const std::vector<VolumeInfo>& getVolumes()
      const noexcept = 0;

  /**
   * @brief Получить список файловых метрик.
   * @return Ссылка на вектор объектов FileMetric.
  */
  [[nodiscard]] virtual const std::vector<FileMetric>& getMetrics()
      const noexcept = 0;
};

}
