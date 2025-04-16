/**
 * @file windows_version.hpp
 * @brief Перечисление версий Windows по формату Prefetch-файла
 */

#pragma once

namespace PrefetchAnalysis {
/**
 * @enum WindowsVersion
 * @brief Детальное перечисление всех известных версий Windows
 * @details Определяет все версии Windows от XP до 11 с учетом сервисных пакетов
 * и полугодовых обновлений, используя номера версий формата Prefetch-файлов
 */
enum class WindowsVersion {
  UNKNOWN = 0,         ///< Неизвестная/неподдерживаемая версия
  WIN_XP = 10,         ///< Windows XP (2001)/Server 2003
  WIN_XP_SP2 = 11,     ///< Windows XP SP2 (2004)
  WIN_XP_SP3 = 12,     ///< Windows XP SP3 (2008)
  WIN_VISTA = 17,      ///< Windows Vista (2006)/Server 2008
  WIN_VISTA_SP1 = 18,  ///< Windows Vista SP1 (2008)
  WIN_VISTA_SP2 = 19,  ///< Windows Vista SP2 (2009)
  WIN7 = 23,           ///< Windows 7 (2009)/Server 2008 R2
  WIN7_SP1 = 24,       ///< Windows 7 SP1 (2011)
  WIN8 = 26,           ///< Windows 8 (2012)/Server 2012
  WIN8_1 = 30,         ///< Windows 8.1 (2013)/Server 2012 R2
  WIN10_1507 = 31,     ///< Windows 10 1507 (Threshold 1, 2015)
  WIN10_1511 = 32,     ///< Windows 10 1511 (Threshold 2, 2015)
  WIN10_1607 = 33,     ///< Windows 10 1607 (Redstone 1, 2016)
  WIN10_1703 = 34,     ///< Windows 10 1703 (Redstone 2, 2017)
  WIN10_1709 = 35,     ///< Windows 10 1709 (Redstone 3, 2017)
  WIN10_1803 = 36,     ///< Windows 10 1803 (Redstone 4, 2018)
  WIN10_1809 = 37,     ///< Windows 10 1809 (Redstone 5, 2018)
  WIN10_1903 = 38,     ///< Windows 10 1903 (19H1, 2019)
  WIN10_1909 = 39,     ///< Windows 10 1909 (19H2, 2019)
  WIN10_2004 = 40,     ///< Windows 10 2004 (20H1, 2020)
  WIN10_20H2 = 41,     ///< Windows 10 20H2 (2020)
  WIN10_21H1 = 42,     ///< Windows 10 21H1 (2021)
  WIN10_21H2 = 43,     ///< Windows 10 21H2 (2021)
  WIN11_21H2 = 44,     ///< Windows 11 21H2 (Sun Valley, 2021)
  WIN11_22H2 = 45,     ///< Windows 11 22H2 (2022)
  WIN11_23H2 = 46,     ///< Windows 11 23H2 (2023)
  WIN11_24H2 = 47      ///< Windows 11 24H2 (2024)
};

/**
 * @brief Преобразует версию Windows в строковое представление
 * @param version Версия Windows из перечисления WindowsVersion
 * @return Строковое представление версии или "Unknown Windows version"
 */
inline std::string_view WindowsVersionToString(
    WindowsVersion version) noexcept {
  switch (version) {
    case WindowsVersion::UNKNOWN:
      return "Unknown Windows version";
    case WindowsVersion::WIN_XP:
      return "Windows XP/Server 2003";
    case WindowsVersion::WIN_XP_SP2:
      return "Windows XP SP2";
    case WindowsVersion::WIN_XP_SP3:
      return "Windows XP SP3";
    case WindowsVersion::WIN_VISTA:
      return "Windows Vista/Server 2008";
    case WindowsVersion::WIN_VISTA_SP1:
      return "Windows Vista SP1";
    case WindowsVersion::WIN_VISTA_SP2:
      return "Windows Vista SP2";
    case WindowsVersion::WIN7:
      return "Windows 7/Server 2008 R2";
    case WindowsVersion::WIN7_SP1:
      return "Windows 7 SP1";
    case WindowsVersion::WIN8:
      return "Windows 8/Server 2012";
    case WindowsVersion::WIN8_1:
      return "Windows 8.1/Server 2012 R2";
    case WindowsVersion::WIN10_1507:
      return "Windows 10 1507 (Threshold 1)";
    case WindowsVersion::WIN10_1511:
      return "Windows 10 1511 (Threshold 2)";
    case WindowsVersion::WIN10_1607:
      return "Windows 10 1607 (Redstone 1)";
    case WindowsVersion::WIN10_1703:
      return "Windows 10 1703 (Redstone 2)";
    case WindowsVersion::WIN10_1709:
      return "Windows 10 1709 (Redstone 3)";
    case WindowsVersion::WIN10_1803:
      return "Windows 10 1803 (Redstone 4)";
    case WindowsVersion::WIN10_1809:
      return "Windows 10 1809 (Redstone 5)";
    case WindowsVersion::WIN10_1903:
      return "Windows 10 1903 (19H1)";
    case WindowsVersion::WIN10_1909:
      return "Windows 10 1909 (19H2)";
    case WindowsVersion::WIN10_2004:
      return "Windows 10 2004 (20H1)";
    case WindowsVersion::WIN10_20H2:
      return "Windows 10 20H2";
    case WindowsVersion::WIN10_21H1:
      return "Windows 10 21H1";
    case WindowsVersion::WIN10_21H2:
      return "Windows 10 21H2";
    case WindowsVersion::WIN11_21H2:
      return "Windows 11 21H2 (Sun Valley)";
    case WindowsVersion::WIN11_22H2:
      return "Windows 11 22H2";
    case WindowsVersion::WIN11_23H2:
      return "Windows 11 23H2";
    case WindowsVersion::WIN11_24H2:
      return "Windows 11 24H2";
    default:
      return "Неизвестная версия Windows";
  }
}

}
