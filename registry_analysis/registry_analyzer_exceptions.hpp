/**
* @file registry_analyzer_exceptions.hpp
 * @brief Пользовательские исключения для анализатора реестра Windows
 */

#pragma once

#include <stdexcept>
#include <string>

namespace RegistryAnalysis {

/**
 * @defgroup Исключения Исключения анализатора реестра
 * @brief Классы для обработки ошибок при работе с реестром
 * @{
 */

/**
 * @class RegistryException
 * @brief Базовое исключение для операций с реестром
 * @details Наследуется от std::runtime_error, содержит сообщение об ошибке
 */
class RegistryException : public std::runtime_error {
 public:
  /**
  * @brief Конструктор исключения
  * @param[in] msg Сообщение об ошибке
  */
  explicit RegistryException(const std::string& msg)
      : std::runtime_error(msg) {}
};

/**
 * @class RegistryInitializationError
 * @brief Ошибка инициализации объекта файла реестра
 * @details Возникает при неудачной попытке создания объекта libregf_file
 */
class RegistryInitializationError : public RegistryException {
 public:
  RegistryInitializationError()
      : RegistryException("Ошибка создания объекта для работы с реестром") {}
};

/**
 * @class FileOpenError
 * @brief Ошибка открытия файла реестра
 * @details Возникает при неудачной попытке открытия файла по указанному пути
 */
class FileOpenError : public RegistryException {
 public:
  /**
  * @brief Конструктор исключения
  * @param[in] path Путь к файлу реестра
  */
  explicit FileOpenError(const std::string& path)
      : RegistryException("Не удалось открыть файл реестра: " + path) {}
};

/**
 * @class RootKeyError
 * @brief Ошибка получения корневого ключа реестра
 * @details Возникает при невозможности получить корневой ключ из файла
 */
class RootKeyError : public RegistryException {
 public:
  RootKeyError()
      : RegistryException("Ошибка получения корневого ключа реестра") {}
};

/** @} */

}  // namespace RegistryAnalysis
