#pragma once

#include <cstddef>
#include <string_view>
#include <cstdint>

/**
 * @defgroup registry_constants Системные константы
 * @brief Параметры формата и ограничения
 * @{
 */

namespace registry {

namespace constants {
/// @name Сигнатуры структур
/// @{
constexpr std::string_view HIVE_SIGNATURE = "regf"; ///< Сигнатура файла
constexpr uint16_t NK_SIGNATURE = 0x6B6E;           ///< 'nk' в little-endian
constexpr uint16_t VK_SIGNATURE = 0x6B76;           ///< 'vk' в little-endian
/// @}

/// @name Версии формата
/// @{
constexpr uint32_t HIVE_VERSION_1 = 1; ///< Windows 2000 - 8.1
constexpr uint32_t HIVE_VERSION_2 = 2; ///< Windows 10+
/// @}

/// @name Смещения в заголовке
/// @{
constexpr size_t HIVE_ROOT_OFFSET = 0x24;     ///< Смещение корневого ключа
constexpr size_t HIVE_SIZE_OFFSET = 0x28;     ///< Размер куста (v1)
constexpr size_t HIVE_SIZE_HIGH_OFFSET = 0x2C;///< Старшие 32 бита размера (v2)
/// @}

/// @name Ограничения
/// @{
constexpr size_t MAX_KEY_NAME_LEN = 255;      ///< Макс. длина имени ключа
constexpr size_t MAX_VALUE_NAME_LEN = 16383;  ///< Макс. длина имени значения
/// @}
}

/// @}
}
