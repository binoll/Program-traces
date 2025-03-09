#pragma once

#include <cstdint>

/**
 * @defgroup registry_types Основные типы данных
 * @brief Типы и перечисления для работы с реестром Windows
 * @{
 */

namespace registry {

/**
 * @brief Типы данных значений реестра (Windows 7+)
 * @details Поддерживает все версии Windows x86/x64
 */
enum class ValueType : uint32_t {
  REG_NONE = 0,               ///< Нет значения
  REG_SZ = 1,                 ///< Строка Unicode с нулевым окончанием
  REG_EXPAND_SZ = 2,          ///< Раскрываемая строка среды
  REG_BINARY = 3,             ///< Произвольные двоичные данные
  REG_DWORD = 4,              ///< 32-битное целое (little-endian)
  REG_DWORD_BIG_ENDIAN = 5,   ///< 32-битное целое (big-endian)
  REG_LINK = 6,               ///< Символическая ссылка Unicode
  REG_MULTI_SZ = 7,           ///< Массив строк с двойным нулём
  REG_RESOURCE_LIST = 8,      ///< Список ресурсов устройства
  REG_FULL_RESOURCE_DESC = 9, ///< Полный дескриптор ресурса
  REG_RESOURCE_REQ_LIST = 10, ///< Список требований к ресурсам
  REG_QWORD = 11,             ///< 64-битное целое (little-endian)
  REG_FILE_NAME = 14,         ///< Имя файла (Windows XP+)
  REG_MUI_STRING = 15,        ///< Многоязычная строка (Vista+)
  REG_VARIABLE_LIST = 0x100,  ///< Список переменных (Win10 1709+)
  REG_UNKNOWN = 0xFFFFFFFF    ///< Неизвестный тип
};

/**
 * @brief Флаги доступа к ключам реестра
 */
namespace KeyAccess {
constexpr uint32_t READ = 0x00020019;    ///< Чтение ключа
constexpr uint32_t WRITE = 0x00020006;   ///< Запись в ключ
constexpr uint32_t WOW64_64KEY = 0x0100; ///< 64-битный просмотр
constexpr uint32_t WOW64_32KEY = 0x0200; ///< 32-битный просмотр
}

/// @}
}
