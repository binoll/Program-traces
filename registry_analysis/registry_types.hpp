#pragma once

#include <cstdint>
#include <string_view>

/**
 * @defgroup registry_types Типы данных реестра
 * @brief Структуры, типы и константы для работы с реестром Windows
 */

/**
 * @ingroup registry_types
 * @brief Типы данных значений реестра
 */
enum class ValueType {
  REG_NONE,   ///< Нулевой тип (отсутствие данных)
  REG_SZ,     ///< Unicode строка с нулевым завершителем
  REG_DWORD,  ///< 32-битное целое число (Little-Endian)
  REG_BINARY  ///< Произвольные бинарные данные
};

/**
 * @ingroup registry_types
 * @brief Узел ключа реестра
 */
struct [[gnu::packed]] KeyNode {
  uint32_t offset;         ///< Абсолютное смещение в файле
  uint32_t subkeys_count;  ///< Количество дочерних ключей
  uint32_t values_count;   ///< Количество значений
  uint32_t last_modified;  ///< Время модификации (UNIX timestamp)
};

/**
 * @ingroup registry_types
 * @brief Запись значения реестра
 */
struct [[gnu::packed]] ValueEntry {
  ValueType type;        ///< Тип значения
  uint32_t data_offset;  ///< Смещение данных
  uint32_t data_size;    ///< Размер данных
  uint16_t name_length;  ///< Длина имени в символах
  uint32_t name_offset;  ///< Смещение имени
};

/**
 * @ingroup registry_types
 * @brief Константы формата HIVE
 */
namespace RegistryConstants {
/// @name Основные параметры
/// @{
constexpr uint32_t HIVE_BASE_OFFSET = 0x1000;    ///< Базовое смещение данных
constexpr size_t HIVE_HEADER_SIZE = 0x200;       ///< Размер заголовка
constexpr std::string_view HIVE_SIGNATURE = "regf"; ///< Сигнатура файла
constexpr uint16_t MAX_VALUE_SIZE = 0xFFFF;      ///< Макс. размер значения
/// @}

/// @name Смещения в заголовке
/// @{
constexpr size_t HIVE_MAJOR_OFFSET = 0x14;  ///< Основная версия
constexpr size_t HIVE_MINOR_OFFSET = 0x18;  ///< Дополнительная версия
constexpr size_t ROOT_KEY_OFFSET = 0x24;    ///< Корневой ключ
/// @}

/// @name Форматы данных
/// @{
constexpr char PATH_DELIMITER = '\\';  ///< Разделитель пути
/// @}
}
