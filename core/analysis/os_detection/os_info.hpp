/// @file os_info.hpp
/// @brief Структура для хранения информации об операционной системе

#pragma once

#include <string>

/// @brief Информация об операционной системе
struct OSInfo {
  std::string product_name;     ///< Название продукта
  std::string current_version;  ///< Версия ОС
  std::string edition_id;       ///< Редакция ОС
  std::string current_build;    ///< Номер сборки
  std::string release_id;       ///< Идентификатор выпуска
  std::string display_version;  ///< Отображаемая версия
  std::string full_os_name;     ///< Реальное название ОС
};
