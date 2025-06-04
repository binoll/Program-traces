/// @file os_info.hpp
/// @brief Перечисление версий Windows по номерам сборок

#pragma once

#include <cstdint>
#include <string>

namespace WindowsVersion {

/// @enum WindowsBuild
/// @brief Перечисление версий Windows по номерам сборок
enum class WindowsBuild : uint32_t {
  UNKNOWN = 0,  ///< Неизвестная версия Windows
  // Клиентские версии
  XP = 2600,           ///< Windows XP (RTM)
  XP_x64 = 3790,       ///< Windows XP Professional x64 Edition
  VISTA = 6000,        ///< Windows Vista (RTM)
  VISTA_SP1 = 6001,    ///< Windows Vista с Service Pack 1
  VISTA_SP2 = 6002,    ///< Windows Vista с Service Pack 2
  WIN7 = 7600,         ///< Windows 7 (RTM)
  WIN7_SP1 = 7601,     ///< Windows 7 с Service Pack 1
  WIN8 = 9200,         ///< Windows 8 (RTM)
  WIN8_1 = 9600,       ///< Windows 8.1 (RTM)
  WIN10_1507 = 10240,  ///< Windows 10 (1507, первоначальный выпуск)
  WIN10_1511 = 10586,  ///< Windows 10 (1511, обновление ноября)
  WIN10_1607 = 14393,  ///< Windows 10 (1607, юбилейное обновление)
  WIN10_1703 = 15063,  ///< Windows 10 (1703, обновление создателей)
  WIN10_1709 = 16299,  ///< Windows 10 (1709, обновление осени создателей)
  WIN10_1803 = 17134,  ///< Windows 10 (1803, обновление апреля 2018)
  WIN10_1809 = 17763,  ///< Windows 10 (1809, обновление октября 2018)
  WIN10_1903 = 18362,  ///< Windows 10 (1903, обновление мая 2019)
  WIN10_1909 = 18363,  ///< Windows 10 (1909, обновление ноября 2019)
  WIN10_2004 = 19041,  ///< Windows 10 (2004, обновление мая 2020)
  WIN10_20H2 = 19042,  ///< Windows 10 (20H2, обновление октября 2020)
  WIN10_21H1 = 19043,  ///< Windows 10 (21H1, обновление мая 2021)
  WIN10_21H2 = 19044,  ///< Windows 10 (21H2, обновление ноября 2021)
  WIN10_22H2 = 19045,  ///< Windows 10 (22H2, обновление октября 2022)
  WIN11_21H2 = 22000,  ///< Windows 11 (21H2, первоначальный выпуск)
  WIN11_22H2 = 22621,  ///< Windows 11 (22H2, обновление 2022)
  WIN11_23H2 = 22631,  ///< Windows 11 (23H2, обновление 2023)
  WIN11_24H2 = 26100,  ///< Windows 11 (24H2, обновление 2024)
  // Серверные версии
  SERVER_2003 = 3790,         ///< Windows Server 2003 (RTM)
  SERVER_2003_R2 = 3790,      ///< Windows Server 2003 R2
  SERVER_2008 = 6001,         ///< Windows Server 2008 (RTM)
  SERVER_2008_SP2 = 6002,     ///< Windows Server 2008 с Service Pack 2
  SERVER_2008_R2 = 7600,      ///< Windows Server 2008 R2 (RTM)
  SERVER_2008_R2_SP1 = 7601,  ///< Windows Server 2008 R2 с Service Pack 1
  SERVER_2012 = 9200,         ///< Windows Server 2012 (RTM)
  SERVER_2012_R2 = 9600,      ///< Windows Server 2012 R2 (RTM)
  SERVER_2016 = 14393,        ///< Windows Server 2016 (RTM)
  SERVER_2019 = 17763,        ///< Windows Server 2019 (RTM)
  SERVER_2022 = 20348,        ///< Windows Server 2022 (RTM)
  SERVER_2025 = 20350         ///< Windows Server 2025 (RTM)
};

/// @struct OSInfo
/// @brief Структура для хранения информации об операционной системе
struct OSInfo {
  std::string product_name;     ///< Официальное название продукта (из реестра)
  std::string real_os_name;     ///< Каноническое название версии Windows
  std::string version;          ///< Основная версия ОС (например, "6.3")
  std::string build_number;     ///< Номер сборки ОС (например, "19045")
  std::string release_id;       ///< Идентификатор выпуска (для Windows 10/11)
  std::string display_version;  ///< Отображаемая версия (например, "22H2")
  std::string edition;          ///< Редакция ОС (например, "Professional")
  WindowsBuild build_version =
      WindowsBuild::UNKNOWN;  ///< Версия сборки из перечисления
};

}
