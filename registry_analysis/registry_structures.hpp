#pragma once
#include "registry_types.hpp"
#include "registry_constants.hpp"

/**
 * @defgroup registry_structures Структуры данных
 * @brief Бинарные структуры формата HIVE
 * @{
 */

namespace registry {

/**
 * @brief Заголовок файла реестра (512 байт)
 * @details Поддерживает все версии Windows (7/10/11 x86/x64)
 */
struct [[gnu::packed]] HiveHeader {
    char signature[4];           ///< Сигнатура "regf"
    uint32_t sequence1;          ///< Контрольная сумма 1
    uint32_t sequence2;          ///< Контрольная сумма 2
    uint64_t last_write_time;    ///< Время последней записи (FILETIME)
    uint32_t major_version;      ///< Основная версия формата
    uint32_t minor_version;      ///< Дополнительная версия
    uint8_t  reserved1[8];       ///< Зарезервировано
    uint32_t root_key_offset;    ///< Смещение корневого ключа

    union {
        struct [[gnu::packed]] { // Версия 1 (Windows 7/8)
            uint32_t hive_size;  ///< Полный размер куста
            uint32_t cluster;    ///< Размер кластера
        } v1;

        struct [[gnu::packed]] { // Версия 2 (Windows 10/11)
            uint64_t hive_size;  ///< 64-битный размер куста
            uint32_t flags;      ///< Флаги формата
            uint32_t reserved2;
        } v2;
    };

    uint8_t reserved3[72];       ///< Зарезервированная область
};

/**
 * @brief Узел ключа реестра (NK-запись)
 * @details Размер структуры 80 байт для x86 и x64
 */
struct [[gnu::packed]] KeyNode {
    uint16_t magic;              ///< Сигнатура 'nk'
    uint16_t flags;              ///< Тип ключа
    uint64_t last_write_time;    ///< Время модификации
    uint32_t access_bits;        ///< Права доступа
    uint32_t parent_offset;      ///< Смещение родительского ключа
    uint32_t subkeys_count;      ///< Число подключаей
    uint32_t volatile_count;     ///< Число временных подключей
    uint32_t subkeys_offset;     ///< Смещение списка подключей
    uint32_t values_count;       ///< Число значений
    uint32_t values_offset;      ///< Смещение списка значений
    uint32_t security_offset;    ///< Смещение дескриптора безопасности
    uint32_t classname_offset;   ///< Смещение имени класса
    uint16_t name_length;        ///< Длина имени ключа (в символах)
    /// Имя ключа в UTF-16LE следует сразу после структуры
};

/// @}
}
