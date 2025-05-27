/// @file volume_info.hpp
/// @brief Классы для работы с метаданными логических томов Windows

#pragma once

#include <cstdint>
#include <string>

#include "../../../exceptions/prefetch/volume_exception.hpp"
#include "volume_type.hpp"

namespace PrefetchAnalysis {

/// @class VolumeInfo
/// @brief Контейнер метаданных логического тома Windows
/// @details Инкапсулирует:
///    - Путь к устройству в формате NT
///    - Серийный номер тома
///    - Время создания
///    - Размер хранилища
///    - Тип тома
class VolumeInfo final {
 public:
  /// @name Основные методы класса
  /// @{

  /// @brief Основной конструктор
  /// @param[in] device_path Путь к устройству в формате "\\Device\\..."
  /// @param[in] serial_number Уникальный серийный номер тома
  /// @param[in] creation_time Время создания в формате FILETIME
  /// @param[in] volume_size Логический размер тома в байтах
  /// @param[in] volume_type Битовая маска типа тома
  /// @note FILETIME: 100-нс интервалы с 01.01.1601
  VolumeInfo(std::string device_path, uint32_t serial_number,
             uint64_t creation_time, uint64_t volume_size = 0,
             uint32_t volume_type = static_cast<uint32_t>(VolumeType::FIXED));

  /// @brief Деструктор по умолчанию
  ~VolumeInfo() = default;
  /// @}

  /// @name Геттеры
  /// @{

  /// @brief Возвращает NT-путь к устройству
  /// @return Константная ссылка на строку с NT-путём в формате "\\Device\\..."
  [[nodiscard]] const std::string& getDevicePath() const noexcept;

  /// @brief Возвращает серийный номер тома
  /// @return Уникальный 32-битный идентификатор тома
  [[nodiscard]] uint32_t getSerialNumber() const noexcept;

  /// @brief Возвращает время создания тома
  /// @return 64-битное значение FILETIME в 100-нс интервалах от 01.01.1601
  /// (UTC)
  [[nodiscard]] uint64_t getCreationTime() const noexcept;

  /// @brief Возвращает логический размер тома
  /// @return Размер тома в байтах (0 если не поддерживается файловой системой)
  [[nodiscard]] uint64_t getVolumeSize() const noexcept;

  /// @brief Возвращает битовую маску типа тома
  /// @return Комбинация флагов VolumeType (FIXED, REMOVABLE, NETWORK и т.д.)
  [[nodiscard]] uint32_t getVolumeType() const noexcept;
  /// @}

  /// @name Проверка типа тома
  /// @{

  /// @brief Проверяет наличие конкретного типа в маске тома
  /// @tparam type Проверяемый тип из VolumeType (шаблонный параметр)
  /// @return true если указанный тип присутствует в маске тома
  template <VolumeType type>
  [[nodiscard]] bool checkVolumeType() const noexcept;

  /// @brief Проверяет несколько типов тома одновременно (битовая маска)
  /// @param[in] types Комбинация флагов VolumeType через битовое ИЛИ
  /// @return true если хотя бы один из указанных типов присутствует
  [[nodiscard]] bool checkVolumeTypes(uint32_t types) const noexcept;
  /// @}

  /// @name Валидация
  /// @{

  /// @brief Выполняет комплексную проверку валидности всех данных тома
  /// @throw InvalidVolumeException Генерируется при обнаружении любых
  /// некорректных данных
  /// @details Проверяет следующие аспекты:
  /// - Соответствие пути формату NT-устройства
  /// - Корректность временной метки создания (не раньше 1601 года)
  /// - Допустимость комбинации флагов типа тома
  /// - Наличие ненулевого серийного номера
  /// @note Время проверяется в формате FILETIME (100-нс интервалы с 01.01.1601)
  void validate() const;

 private:
  /// @brief Проверяет корректность NT-пути к устройству
  /// @return true если путь:
  /// - Начинается с \\Device\\
  /// - Содержит одно из ключевых слов устройств
  /// - Не содержит запрещённых символов
  /// @note Поддерживаемые ключевые слова: HarddiskVolume, CdRom и др.
  [[nodiscard]] bool validatePath() const;

  /// @brief Проверяет валидность битовой маски типа тома
  /// @return true если:
  /// - Не установлены неизвестные флаги
  /// - Сочетание флагов допустимо
  /// @note Использует предопределённую маску VALID_VOLUME_TYPES_MASK
  [[nodiscard]] bool validateVolumeType() const noexcept;
  /// @}

  std::string device_path_;  ///< NT-путь к устройству
  uint32_t serial_number_;   ///< Уникальный идентификатор тома
  uint64_t creation_time_;   ///< Время создания (FILETIME)
  uint64_t volume_size_;     ///< Логический размер в байтах
  uint32_t volume_type_;     ///< Битовая маска типа тома
};

}
