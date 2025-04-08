/**
 * @file volume.hpp
 * @brief Заголовочный файл класса VolumeInfo для работы с метаданными логических томов
*/

#pragma once

#include <cstdint>
#include <string>
#include "../exceptions.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Класс для хранения и управления метаданными логического тома
 * @details Инкапсулирует информацию о NT-пути, серийном номере и времени создания тома.
 *          Гарантирует целостность данных через проверки в конструкторе.
*/
class VolumeInfo {
 public:
  /**
   * @brief Конструктор с полной инициализацией параметров
   * @param[in] device_path NT-путь в формате "\Device\HarddiskVolumeX"
   * @param[in] serial 32-битный серийный номер тома
   * @param[in] create_time Время создания в формате FILETIME
   *
   * @throw InvalidDevicePathException Если device_path пуст или содержит только пробелы
  */
  VolumeInfo(std::string device_path, uint32_t serial, uint64_t create_time);

  /// @name Геттеры свойств
  /// @{

  /**
   * @brief Получить NT-путь к устройству
   * @return Константная копия строки с NT-путем
  */
  [[nodiscard]] std::string device_path() const noexcept;

  /**
   * @brief Получить серийный номер тома
   * @return 32-битное беззнаковое целое - уникальный идентификатор
   * @note Значение соответствует полю METADATA::SerialNumber в структурах Prefetch
  */
  [[nodiscard]] uint32_t serial_number() const noexcept;

  /**
   * @brief Получить временную метку создания
   * @return 64-битное целое в формате FILETIME (100-нс интервалы с 1601-01-01)
   * @warning Для конвертации в UNIX-время требуется учет часовых поясов
  */
  [[nodiscard]] uint64_t creation_time() const noexcept;
  /// @}

 private:
  std::string device_path_;  ///< NT-путь к устройству хранения данных
  uint32_t serial_number_;   ///< Уникальный идентификатор тома
  uint64_t creation_time_;   ///< Временная метка инициализации тома
};

}
