#pragma once

#include "registry_structures.hpp"

/**
 * @defgroup registry_utils Утилиты
 * @brief Вспомогательные функции для работы с реестром
 * @{
 */

namespace registry {

/**
 * @brief Определение архитектуры куста реестра
 * @param header Указатель на заголовок HIVE
 * @return true если куст 64-битный
 */
inline bool is_64bit_hive(const HiveHeader* header) noexcept {
  return (header->major_version >= constants::HIVE_VERSION_2) ||
         (header->v2.hive_size > UINT32_MAX);
}

/**
 * @brief Получение полного размера куста
 * @param header Указатель на заголовок HIVE
 * @return Полный размер в байтах
 */
inline uint64_t get_hive_size(const HiveHeader* header) noexcept {
  return (header->major_version == constants::HIVE_VERSION_2) ?
      header->v2.hive_size : header->v1.hive_size;
}

/**
 * @brief Проверка поддержки Windows 10+
 * @param header Указатель на заголовок HIVE
 * @return true если куст создан в Windows 10 или новее
 */
inline bool is_win10_or_later(const HiveHeader* header) noexcept {
  return header->major_version >= constants::HIVE_VERSION_2;
}

/// @}
}
