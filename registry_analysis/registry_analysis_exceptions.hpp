#pragma once

#include <stdexcept>
#include <string>

/**
 * @defgroup registry_exceptions Исключения анализа реестра
 * @brief Иерархия исключений для обработки ошибок
 * @ingroup registry_analysis
 */

/**
 * @ingroup registry_exceptions
 * @brief Базовое исключение операций с реестром
 */
class RegistryException : public std::runtime_error {
 public:
  /**
  * @param msg Детализированное описание ошибки
  */
  explicit RegistryException(const std::string& msg)
      : std::runtime_error("Ошибка реестра: " + msg) {}
};

/**
 * @ingroup registry_exceptions
 * @brief Ошибка валидации HIVE-файла
 */
class HiveValidationError : public RegistryException {
 public:
  /**
  * @param details Причина ошибки валидации
  */
  explicit HiveValidationError(const std::string& details)
      : RegistryException("Ошибка HIVE: " + details) {}
};

/**
 * @ingroup registry_exceptions
 * @brief Ошибка отсутствия ключа
 */
class KeyNotFoundError : public RegistryException {
 public:
  /**
  * @param key Путь к отсутствующему ключу
  */
  explicit KeyNotFoundError(const std::string& key)
      : RegistryException("Ключ не найден: " + key) {}
};

/**
 * @ingroup registry_exceptions
 * @brief Ошибка отсутствия значения
 */
class ValueNotFoundError : public RegistryException {
 public:
  /**
  * @param value Имя отсутствующего значения
  */
  explicit ValueNotFoundError(const std::string& value)
      : RegistryException("Значение не найдено: " + value) {}
};

/**
 * @ingroup registry_exceptions
 * @brief Ошибка преобразования данных
 */
class EncodingError : public RegistryException {
 public:
  /**
  * @param operation Название операции преобразования
  */
  explicit EncodingError(const std::string& operation)
      : RegistryException("Ошибка кодирования: " + operation) {}
};
