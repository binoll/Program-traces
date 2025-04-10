/**
 * @file volume_info.hpp
 * @brief Классы для работы с метаданными логических томов.
*/

#pragma once

#include <cstdint>
#include <string>
#include "../../exceptions/prefetch/volume_exception.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Проверяет валидность NT-пути.
 * @details Функция проверяет, является ли переданный путь непустым и не состоящим из пробелов.
 * @param path Проверяемый путь.
 * @return true, если путь валиден, иначе false.
*/
[[nodiscard]] static bool is_valid(const std::string& path) noexcept;

/**
 * @class VolumeInfo
 * @brief Хранилище метаданных логического тома.
 * @details Этот класс инкапсулирует информацию о логическом томе, включая:
 *          - Путь к устройству в формате NT.
 *          - Уникальный серийный номер тома.
 *          - Время создания тома (в формате FILETIME).
*/
class VolumeInfo final {
 public:
  /**
   * @brief Конструктор с валидацией пути.
   * @details Инициализирует объект класса VolumeInfo, проверяя валидность переданного пути.
   * @param device_path NT-путь к устройству, который будет проверен на валидность.
   * @param serial Уникальный серийный номер тома.
   * @param create_time Время создания тома в формате FILETIME.
   * @throw InvalidDevicePathException Исключение выбрасывается, если путь к устройству некорректен.
  */
  VolumeInfo(std::string device_path, uint32_t serial, uint64_t create_time);

  /**
   * @brief Получить NT-путь к устройству.
   * @return Константная ссылка на строку с NT-путем устройства.
  */
  [[nodiscard]] const std::string& getDevicePath() const noexcept;

  /**
   * @brief Получить серийный номер тома.
   * @return 32-битный беззнаковый целочисленный идентификатор тома.
  */
  [[nodiscard]] uint32_t getSerialNumber() const noexcept;

  /**
   * @brief Получить временную метку создания тома.
   * @return 64-битное значение времени в формате FILETIME.
  */
  [[nodiscard]] uint64_t getCreationTime() const noexcept;

 private:
  const std::string device_path_;  ///< NT-путь к устройству.
  const uint32_t serial_number_;   ///< Уникальный идентификатор тома.
  const uint64_t
      creation_time_;  ///< Время инициализации тома в формате FILETIME.
};

}
