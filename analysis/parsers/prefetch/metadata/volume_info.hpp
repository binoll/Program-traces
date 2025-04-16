/**
 * @file volume_info.hpp
 * @brief Класс для работы с метаданными томов
 */

#pragma once

#include <cstdint>
#include <string>

#include "../../../exceptions/prefetch/volume_exception.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Проверяет валидность NT-пути к устройству
 * @param[in] path Путь для проверки в формате NT ("\Device\HarddiskVolume1")
 * @return true если путь не пустой и содержит допустимые символы, иначе false
 * @note Проверяет только синтаксис пути, не существование устройства
 */
[[nodiscard]] static bool is_valid(const std::string& path) noexcept;

/**
 * @class VolumeInfo
 * @brief Класс для хранения и обработки метаданных логического тома
 * @note Инкапсулирует полную информацию о томе, включая системные
 * идентификаторы, временные метки и физические характеристики. Поддерживает все
 * типы томов (локальные, сетевые, съемные)
 */
class VolumeInfo final {
 public:
  /// @name Константы типов томов
  /// @{
  static constexpr uint32_t VOLUME_TYPE_FIXED =
      0x01;  ///< Локальный несъемный диск
  static constexpr uint32_t VOLUME_TYPE_REMOVABLE =
      0x02;                                              ///< Съемное устройство
  static constexpr uint32_t VOLUME_TYPE_NETWORK = 0x04;  ///< Сетевой диск
  static constexpr uint32_t VOLUME_TYPE_OPTICAL = 0x08;  ///< Оптический привод
  static constexpr uint32_t VOLUME_TYPE_RAMDISK = 0x10;  ///< RAM-диск
  /// @}

  /// @name Основные методы класса
  /// @{
  /**
   * @brief Конструктор с полной инициализацией метаданных тома
   * @param[in] device_path NT-путь к устройству
   * @param[in] serial Уникальный серийный номер тома
   * @param[in] create_time Время создания в формате FILETIME
   * @param[in] size Общий размер тома в байтах (по умолчанию 0)
   * @param[in] type Битовая маска типа тома (по умолчанию FIXED)
   * @throw InvalidDevicePathException При некорректном NT-пути
   * @note Время создания задается в 100-наносекундных интервалах с 01011601
   */
  VolumeInfo(std::string device_path, uint32_t serial, uint64_t create_time,
             uint64_t size = 0, uint32_t type = VOLUME_TYPE_FIXED);

  /**
   * @brief Деструктор по умолчанию
   */
  ~VolumeInfo() = default;
  /// @}

  /// @name Методы валидации
  /// @{
  /**
   * @brief Проверяет корректность метаданных тома
   * @return true если все параметры тома валидны:
   * - NT-путь устройства соответствует синтаксису
   * - Серийный номер не равен 0
   * - Указан хотя бы один допустимый тип тома
   * - Время создания > 01.01.1601
   * @note Метод проверяет только формальную корректность данных,
   * но не гарантирует фактическое существование тома в системе
   */
  [[nodiscard]] bool isValid() const noexcept;
  /// @}

  /// @name Базовые методы доступа
  /// @{
  /**
   * @brief Получить NT-путь к устройству
   * @return Константная ссылка на строку с NT-путем устройства
   */
  [[nodiscard]] const std::string& getDevicePath() const noexcept;

  /**
   * @brief Получить серийный номер тома
   * @return 32-битный беззнаковый целочисленный идентификатор тома
   */
  [[nodiscard]] uint32_t getSerialNumber() const noexcept;

  /**
   * @brief Получить временную метку создания тома
   * @return 64-битное значение времени в формате FILETIME
   */
  [[nodiscard]] uint64_t getCreationTime() const noexcept;
  /// @}

  /// @name Методы работы с характеристиками тома
  /// @{
  /**
   * @brief Получить полный размер тома
   * @return Размер тома в байтах
   * @note Возвращает 0, если размер неизвестен
   */
  [[nodiscard]] uint64_t getVolumeSize() const noexcept;

  /**
   * @brief Получить тип тома
   * @return Битовая маска типа тома
   * @see VOLUME_TYPE_FIXED, VOLUME_TYPE_REMOVABLE, VOLUME_TYPE_NETWORK
   */
  [[nodiscard]] uint32_t getVolumeType() const noexcept;

  /**
   * @brief Проверить, является ли том съемным устройством
   * @return true если установлен флаг VOLUME_TYPE_REMOVABLE, иначе false
   */
  [[nodiscard]] bool isRemovable() const noexcept;

  /**
   * @brief Проверить, является ли том сетевым диском
   * @return true если установлен флаг VOLUME_TYPE_NETWORK, иначе false
   */
  [[nodiscard]] bool isNetworkDrive() const noexcept;
  /// @}

 private:
  std::string device_path_;  ///< NT-путь в формате "\Device\HarddiskVolumeX"
  uint32_t serial_number_;   ///< Уникальный серийный номер
  uint64_t creation_time_;   ///< Время создания (FILETIME)
  uint64_t volume_size_;     ///< Полный размер в байтах
  uint32_t volume_type_;     ///< Битовая маска типа тома
};

}
