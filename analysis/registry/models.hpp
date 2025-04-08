/**
 * @file models.hpp
 * @brief Модели данных для работы с реестром
 * @details Содержит структуры данных для представления элементов реестра.
*/

#pragma once

#include <cstdint>
#include <string>

namespace RegistryParser {

/**
 * @struct RegistryValue
 * @brief Контейнер данных значения реестра
 * @details Содержит имя, данные и тип значения в унифицированном формате.
*/
struct RegistryValue {
  std::string name_;  ///< Имя значения в кодировке UTF-8
  std::string data_;  ///< Данные в строковом представлении
  uint32_t type_;     ///< Тип значения (REG_SZ=1, REG_DWORD=4, REG_QWORD=11)
};

}
