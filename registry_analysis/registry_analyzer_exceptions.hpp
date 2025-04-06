/**
 * @file registry_analyzer_exceptions.hpp
 * @brief Пользовательские исключения для анализатора реестра Windows
 * @details Содержит классы исключений для обработки ошибок при работе с реестром
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
 */
class RegistryException : public std::runtime_error {
 public:
  explicit RegistryException(const std::string& msg)
      : std::runtime_error(msg) {}
};

/**
 * @class RegistryInitializationError
 * @brief Ошибка инициализации объекта файла реестра
 */
class RegistryInitializationError : public RegistryException {
 public:
  RegistryInitializationError()
      : RegistryException("Ошибка создания объекта для работы с реестром") {}
};

/**
 * @class FileOpenError
 * @brief Ошибка открытия файла реестра
 */
class FileOpenError : public RegistryException {
 public:
  explicit FileOpenError(const std::string& path)
      : RegistryException("Не удалось открыть файл: " + path) {}
};

/**
 * @class RootKeyError
 * @brief Ошибка получения корневого ключа реестра
 */
class RootKeyError : public RegistryException {
 public:
  RootKeyError() : RegistryException("Ошибка получения корневого ключа") {}
};

/**
 * @class SubkeyNotFoundError
 * @brief Ошибка поиска подраздела реестра
 */
class SubkeyNotFoundError : public RegistryException {
 public:
  explicit SubkeyNotFoundError(const std::string& name)
      : RegistryException("Подраздел '" + name + "' не найден") {}
};

/**
 * @class ValueNotFoundError
 * @brief Ошибка поиска значения реестра
 */
class ValueNotFoundError : public RegistryException {
 public:
  explicit ValueNotFoundError(const std::string& name)
      : RegistryException("Значение '" + name + "' не найдено") {}
};

/** @} */

}  // namespace RegistryAnalysis
