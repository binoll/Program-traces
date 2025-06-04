/// @file os_info.hpp
/// @brief Структура для хранения информации об операционной системе

#pragma once

#include <string>

/// @brief Версии сборок Windows
enum class WindowsBuild {
  UNKNOWN,
  XP,
  XP_x64,
  VISTA,
  VISTA_SP1,
  VISTA_SP2,
  WIN7,
  WIN7_SP1,
  WIN8,
  WIN8_1,
  WIN10_1507,
  WIN10_1511,
  WIN10_1607,
  WIN10_1703,
  WIN10_1709,
  WIN10_1803,
  WIN10_1809,
  WIN10_1903,
  WIN10_1909,
  WIN10_2004,
  WIN10_20H2,
  WIN10_21H1,
  WIN10_21H2,
  WIN10_22H2,
  WIN11_21H2,
  WIN11_22H2,
  WIN11_23H2,
  WIN11_24H2,
  SERVER_2003,
  SERVER_2008,
  SERVER_2008_SP2,
  SERVER_2008_R2,
  SERVER_2008_R2_SP1,
  SERVER_2012,
  SERVER_2012_R2,
  SERVER_2016,
  SERVER_2019,
  SERVER_2022,
  SERVER_2025
};

/// @brief Информация об операционной системе
struct OSInfo {
  std::string product_name;     ///< Название продукта
  std::string version;          ///< Версия ОС
  std::string edition;          ///< Редакция ОС
  std::string build_number;     ///< Номер сборки
  std::string build_id;         ///< Номер сборки
  std::string release;          ///< Идентификатор выпуска
  std::string display_version;  ///< Отображаемая версия
  std::string full_os_name;     ///< Реальное название ОС
  WindowsBuild build_version;   ///< Версия сборки
};
