#pragma once

#include <cstdint>
#include <string_view>

/**
 * @defgroup registry_parser Парсер реестра Windows
 * @brief Набор структур и утилит для чтения файлов реестра Windows (HIVE)
 * @details Реализация основана на официальной документации формата реестра
 */

/**
 * @ingroup registry_parser
 * @defgroup registry_types Типы данных реестра
 * @brief Основные структуры данных и константы формата HIVE
 */

/**
 * @ingroup registry_types
 * @brief Типы данных значений реестра
 */
enum class ValueType : uint32_t {
  REG_NONE = 0,              ///< Нет значения
  REG_SZ = 1,                ///< Unicode строка с нулевым завершителем
  REG_EXPAND_SZ = 2,         ///< Раскрываемая строка (переменные среды)
  REG_BINARY = 3,            ///< Бинарные данные произвольного формата
  REG_DWORD = 4,             ///< 32-битное целое (Little-Endian)
  REG_DWORD_BIG_ENDIAN = 5,  ///< 32-битное целое (Big-Endian)
  REG_LINK = 6,              ///< Символическая ссылка Unicode
  REG_MULTI_SZ = 7,          ///< Массив строк с двойным нулевым завершителем
  REG_RESOURCE_LIST = 8,     ///< Список ресурсов устройства
  REG_QWORD = 11,            ///< 64-битное целое (Little-Endian)
  REG_UNKNOWN = 0xFFFFFFFF   ///< Неизвестный или ошибочный тип
};

/**
 * @ingroup registry_types
 * @brief Структура узла ключа реестра (nk-запись)
 * @details Содержит метаинформацию о ключе реестра. Имя ключа хранится сразу после структуры.
 * @warning Все смещения рассчитываются от начала файла HIVE
 */
struct [[gnu::packed]] KeyNode {
  uint16_t magic;                   ///< Сигнатура "nk" (0x6B6E)
  uint16_t type;                    ///< Тип ключа (см. KeyType)
  uint64_t last_modified;           ///< Время модификации в формате FILETIME
  uint32_t access_bits;             ///< Флаги доступа (см. KeyAccessRights)
  uint32_t parent_offset;           ///< Смещение родительского ключа
  uint32_t subkeys_count;           ///< Количество стабильных подключаей
  uint32_t volatile_subkeys_count;  ///< Количество временных подключей
  uint32_t subkeys_offset;          ///< Смещение списка подключей
  uint32_t values_count;            ///< Количество значений в ключе
  uint32_t values_offset;           ///< Смещение списка значений
  uint32_t sk_offset;               ///< Смещение дескриптора безопасности
  uint32_t classname_offset;  ///< Смещение имени класса (0 если отсутствует)
  uint16_t name_length;       ///< Длина имени ключа в символах UTF-16
  /// @note Имя ключа в формате UTF-16LE следует сразу после структуры
};

/**
 * @ingroup registry_types
 * @brief Флаги типов ключей реестра
 */
namespace KeyType {
constexpr uint16_t ROOT = 0x2C;      ///< Корневой ключ куста
constexpr uint16_t REGULAR = 0x2C;   ///< Обычный ключ
constexpr uint16_t VOLATILE = 0x20;  ///< Временный ключ (не сохраняется)
constexpr uint16_t SYMLINK = 0x40;   ///< Символическая ссылка
}  // namespace KeyType

/**
 * @ingroup registry_types
 * @brief Запись значения реестра (vk-запись)
 * @details Содержит метаинформацию о значении ключа. Имя значения (если есть)
 *          хранится сразу после структуры.
 */
struct [[gnu::packed]] ValueEntry {
  uint16_t magic;  ///< Сигнатура "vk" (0x6B76)
  uint16_t
      name_length;  ///< Длина имени в символах UTF-16 (0 для значения по умолчанию)
  uint32_t data_size;  ///< Размер данных в байтах
  union {
    uint32_t data_offset;  ///< Смещение данных (для встроенных данных)
    uint64_t
        big_data_offset;  ///< 64-битное смещение (Win10+ для больших кустов)
  };
  ValueType type;     ///< Тип значения
  uint16_t flags;     ///< Флаги хранения (см. ValueStorageFlags)
  uint16_t reserved;  ///< Зарезервировано для выравнивания
  /// @note Имя значения в формате UTF-16LE следует сразу после структуры
};

/**
 * @ingroup registry_types
 * @brief Флаги хранения значений реестра
 */
namespace ValueStorageFlags {
constexpr uint16_t INLINE = 0x0001;    ///< Данные хранятся внутри записи
constexpr uint16_t EXTENDED = 0x0002;  ///< Используется 64-битное смещение
}  // namespace ValueStorageFlags

/**
 * @ingroup registry_types
 * @brief Заголовок файла реестра (HIVE)
 * @details Первые 512 байт файла. Все поля Little-Endian.
 */
struct [[gnu::packed]] HiveHeader {
  char signature[4];   ///< Всегда "regf" (0x66676572)
  uint32_t sequence1;  ///< Контрольная сумма 1
  uint32_t sequence2;  ///< Контрольная сумма 2 (должна совпадать с sequence1)
  uint64_t timestamp;  ///< Время создания куста (FILETIME)
  uint32_t
      major_version;  ///< Основная версия формата (1 - до Win10, 2 - Win10+)
  uint32_t minor_version;    ///< Дополнительная версия
  uint8_t unknown[8];        ///< Неиспользуемое поле
  uint32_t root_key_offset;  ///< Смещение корневого ключа
  uint32_t hive_size;        ///< Размер куста в байтах (32-бит для версии 1)
  uint32_t hive_size_high;   ///< Старшие 32 бита размера (для версии 2+)
  uint8_t reserved[72];      ///< Зарезервированная область
};

/**
 * @ingroup registry_parser
 * @brief Константы и параметры формата HIVE
 */
namespace RegistryConstants {
/// @name Сигнатуры структур
/// @{
constexpr std::string_view HIVE_SIGNATURE = "regf";  ///< Сигнатура заголовка
constexpr std::string_view NK_SIGNATURE = "nk";      ///< Сигнатура ключа
constexpr std::string_view VK_SIGNATURE = "vk";      ///< Сигнатура значения
/// @}

/// @name Смещения в заголовке HIVE
/// @{
constexpr size_t HIVE_SIGNATURE_OFFSET = 0x0000;  ///< Смещение сигнатуры
constexpr size_t HIVE_SEQUENCE1_OFFSET = 0x0004;  ///< Первая контрольная сумма
constexpr size_t HIVE_SEQUENCE2_OFFSET = 0x01FC;  ///< Вторая контрольная сумма
constexpr size_t HIVE_TIMESTAMP_OFFSET = 0x000C;  ///< Время создания куста
constexpr size_t HIVE_VERSION_OFFSET = 0x0014;    ///< Версия формата
constexpr size_t ROOT_KEY_OFFSET = 0x0024;        ///< Смещение корневого ключа
/// @}

/// @name Параметры данных
/// @{
constexpr uint32_t HIVE_BASE_OFFSET = 0x1000;  ///< Базовое смещение данных
constexpr size_t MAX_INLINE_DATA_SIZE =
    0x8000;                                  ///< Макс. размер встроенных данных
constexpr wchar_t REG_SZ_DELIMITER = L'\0';  ///< Завершитель строки
constexpr wchar_t REG_MULTI_SZ_DELIMITER =
    L'\0';  ///< Двойной завершитель строк
/// @}

/// @name Версии Windows
/// @{
constexpr uint32_t WIN10_THRESHOLD = 0xA0000001;  ///< Минимальная версия Win10
constexpr uint32_t WIN11_THRESHOLD = 0xA0000002;  ///< Минимальная версия Win11
/// @}
}  // namespace RegistryConstants

/**
 * @ingroup registry_parser
 * @brief Утилиты для работы с реестром
 */
namespace RegistryUtils {
/**
 * @brief Конвертирует FILETIME в UNIX-время
 * @param ft 64-битное значение FILETIME
 * @return UNIX-время в секундах
 */
inline uint64_t filetime_to_unix(uint64_t ft) {
  return (ft / 10000000ULL) - 11644473600ULL;
}

/**
 * @brief Проверяет валидность заголовка HIVE
 * @param header Указатель на заголовок
 * @return true если заголовок валиден
 */
inline bool validate_header(const HiveHeader* header) {
  return memcmp(header->signature, "regf", 4) == 0 &&
         header->sequence1 == header->sequence2 && header->major_version >= 1 &&
         header->major_version <= 2;
}

/**
 * @brief Получает полный размер куста
 * @param header Указатель на заголовок
 * @return Размер в байтах (64-бит для версии 2+)
 */
inline uint64_t get_full_hive_size(const HiveHeader* header) {
  if (header->major_version >= 2) {
    return (static_cast<uint64_t>(header->hive_size_high) << 32) |
           header->hive_size;
  }
  return header->hive_size;
}
}  // namespace RegistryUtils
