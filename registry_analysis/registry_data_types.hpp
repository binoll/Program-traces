#pragma once

#include <cstdint>

/**
 * @defgroup registry_types Типы данных реестра
 * @brief Основные структуры и типы данных для работы с реестром Windows
 */

/**
 * @ingroup registry_types
 * @brief Типы данных значений реестра
 * @details Стандартные типы данных, используемые в системном реестре Windows
 */
enum class ValueType {
  REG_NONE,   ///< Пустое значение или отсутствие данных
  REG_SZ,     ///< Строка в кодировке UTF-16LE с нулевым завершителем
  REG_DWORD,  ///< 32-битное целое число (Little-Endian)
  REG_BINARY  ///< Произвольные бинарные данные
};

/**
 * @ingroup registry_types
 * @brief Структура узла ключа реестра
 * @details Содержит метаданные раздела реестра в бинарном формате
 */
struct [[gnu::packed]] KeyNode {
  uint32_t offset;         ///< Абсолютное смещение в файле (байты)
  uint32_t subkeys_count;  ///< Количество дочерних подразделов
  uint32_t values_count;   ///< Количество значений в разделе
  uint32_t last_modified;  ///< Время модификации (Unix timestamp)
};

/**
 * @ingroup registry_types
 * @brief Структура записи значения реестра
 * @details Описывает расположение данных значения в файле
 */
struct [[gnu::packed]] ValueEntry {
  ValueType type;        ///< Тип значения из перечисления ValueType
  uint32_t data_offset;  ///< Смещение данных значения (байты)
  uint32_t data_size;    ///< Размер данных значения (байты)
  uint16_t name_length;  ///< Длина имени в символах UTF-16
  uint32_t name_offset;  ///< Смещение имени значения (байты)
};

/**
 * @ingroup registry_types
 * @brief Константы формата HIVE-файла
 * @details Параметры формата файлов системного реестра
 */
struct [[gnu::packed]] RegistryEntry {
  /// @brief Базовое смещение данных (4096 байт)
  static constexpr uint32_t HIVE_BASE_OFFSET = 0x1000;

  /// @brief Максимальный размер значения (65535 байт)
  static constexpr uint16_t MAX_VALUE_SIZE = 0xFFFF;

  /// @brief Размер заголовка HIVE (512 байт)
  static constexpr size_t HIVE_HEADER_SIZE = 0x200;

  /// @brief Сигнатура файла ("regf")
  static constexpr std::string_view HIVE_SIGNATURE = "regf";
};
