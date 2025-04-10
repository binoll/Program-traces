/**
 * @file prefetch_data_builder.hpp
 * @brief Интерфейс для построения объектов PrefetchData с валидацией и сборкой данных.
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
 * @class PrefetchDataBuilder
 * @brief Класс-строитель для создания объектов PrefetchData с проверкой и валидацией.
 *
 * @details Используется для пошаговой сборки объекта PrefetchData с возможностью цепочек вызовов.
 * Поддерживает валидацию обязательных параметров.
*/
class PrefetchDataBuilder {
 public:
  /**
   * @brief Конструктор по умолчанию.
  */
  PrefetchDataBuilder() = default;

  /**
   * @brief Установить имя исполняемого файла.
   * @param name Имя файла в формате "ИМЯ.EXE-ХЕШ".
   * @return Ссылка на текущий объект PrefetchDataBuilder.
  */
  PrefetchDataBuilder& setExecutableName(std::string name);

  /**
   * @brief Установить хеш Prefetch-файла.
   * @param hash 32-битный хеш.
   * @return Ссылка на текущий объект PrefetchDataBuilder.
  */
  PrefetchDataBuilder& setPrefetchHash(uint32_t hash) noexcept;

  /**
   * @brief Установить количество запусков.
   * @param count Целое число.
   * @return Ссылка на текущий объект PrefetchDataBuilder.
  */
  PrefetchDataBuilder& setRunCount(uint32_t count) noexcept;

  /**
   * @brief Добавить временную метку запуска.
   * @param time Временная метка в формате UNIX.
   * @return Ссылка на текущий объект PrefetchDataBuilder.
  */
  PrefetchDataBuilder& addRunTime(time_t time) noexcept;

  /**
   * @brief Добавить объект тома.
   * @param vol Объект типа VolumeInfo.
   * @return Ссылка на текущий объект PrefetchDataBuilder.
  */
  PrefetchDataBuilder& addVolume(VolumeInfo vol);

  /**
   * @brief Добавить файловую метрику.
   * @param metric Объект типа FileMetric.
   * @return Ссылка на текущий объект PrefetchDataBuilder.
  */
  PrefetchDataBuilder& addMetric(FileMetric metric);

  /**
   * @brief Завершить сборку и получить готовый объект PrefetchData.
   * @return Указатель на IPrefetchData.
   * @throws InvalidExecutableNameException если имя исполняемого файла не задано или некорректно.
  */
  [[nodiscard]] std::unique_ptr<IPrefetchData> build();

 private:
  std::string
      executable_name_;  ///< Имя исполняемого файла (обязательный параметр).
  uint32_t prefetch_hash_ = 0;       ///< 32-битный хеш из заголовка Prefetch.
  uint32_t run_count_ = 0;           ///< Количество запусков (по умолчанию 0).
  std::vector<time_t> run_times_;    ///< Список временных меток запусков.
  std::vector<VolumeInfo> volumes_;  ///< Список логических томов.
  std::vector<FileMetric> metrics_;  ///< Список метрик доступа к файлам.
};

}
