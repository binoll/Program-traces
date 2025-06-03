/// @file registry_exception.hpp
/// @brief Исключения для работы с реестром Windows

#pragma once

#include <fmt/format.h>

#include <stdexcept>
#include <string>

namespace RegistryAnalysis {

/// @class RegistryException
/// @brief Базовый класс для ошибок, связанных с реестром
/// @details Является базовым классом для всех исключений при работе с реестром
class RegistryException : public std::runtime_error {
 public:
  explicit RegistryException(const std::string& message)
      : std::runtime_error(message) {}
};

/// @class RootKeyError
/// @brief Ошибка доступа к корневому разделу реестра
class RootKeyError : public RegistryException {
 public:
  explicit RootKeyError(const std::string& details = "")
      : RegistryException("Ошибка доступа к корневому разделу реестра" +
                          (details.empty() ? "" : ": " + details)) {}
};

/// @class KeyNotFoundError
/// @brief Ошибка: подраздел реестра не найден
class KeyNotFoundError : public RegistryException {
 public:
  explicit KeyNotFoundError(const std::string& name,
                            const std::string& path = "")
      : RegistryException("Подраздел реестра \"" + name + "\" не найден" +
                          (path.empty() ? "" : " по пути: " + path)) {}
};

/// @class ValueNotFoundError
/// @brief Ошибка: значение в реестре не найдено
class ValueNotFoundError : public RegistryException {
 public:
  explicit ValueNotFoundError(const std::string& name,
                              const std::string& key_path = "")
      : RegistryException("Значение \"" + name + "\" не найдено" +
                          (key_path.empty() ? "" : " в разделе: " + key_path)) {
  }
};

/// @class BinaryDataReadError
/// @brief Ошибка чтения бинарных данных из реестра
class BinaryDataReadError : public RegistryException {
 public:
  explicit BinaryDataReadError(const std::string& details = "")
      : RegistryException("Ошибка чтения бинарных данных" +
                          (details.empty() ? "" : ": " + details)) {}
};

class InvalidType : public RegistryException {
 public:
  explicit InvalidType(const uint32_t type)
      : RegistryException(
            "Некорректный тип данных реестра (в числовом представлении): " +
            std::to_string(type)) {}
};

/// @class InvalidValueAccess
/// @brief Ошибка доступа к значению несоответствующего типа
class InvalidValueAccess : public RegistryException {
 public:
  explicit InvalidValueAccess(const std::string& expected_type,
                              const std::string& actual_type,
                              const std::string& value_name = "")
      : RegistryException(fmt::format("Некорректный доступ к значению \"{}\": "
                                      "ожидался тип {}, фактический тип {}",
                                      value_name, expected_type, actual_type)) {
  }
};

/// @class ValueConversionError
/// @brief Ошибка преобразования значения реестра
class ValueConversionError : public RegistryException {
 public:
  explicit ValueConversionError(const std::string& value_name,
                                const std::string& details)
      : RegistryException(fmt::format("Ошибка преобразования значения \"{}\": {}",
                                      value_name, details)) {}
};

/// @class UnsupportedTypeError
/// @brief Ошибка неподдерживаемого типа значения реестра
class UnsupportedTypeError : public RegistryException {
 public:
  explicit UnsupportedTypeError(uint32_t type)
      : RegistryException(
            fmt::format("Неподдерживаемый тип данных реестра: 0x{:X}", type)) {}
};

/// @class TypeCompatibilityError
/// @brief Ошибка несовместимости типа данных и значения
class TypeCompatibilityError : public RegistryException {
 public:
  explicit TypeCompatibilityError(const std::string& expected_type,
                                  const std::string& actual_type,
                                  const std::string& value_name = "")
      : RegistryException(fmt::format("Несовместимость типов для значения "
                                      "\"{}\": ожидался {}, фактический {}",
                                      value_name, expected_type, actual_type)) {
  }
};

/// @class RegistryNotOpenError
/// @brief Ошибка доступа к неоткрытому реестру
class RegistryNotOpenError : public RegistryException {
 public:
  explicit RegistryNotOpenError(const std::string& details)
      : RegistryException(
            fmt::format("Ошибка доступа к неоткрытому реестру: {}", details)) {}
};

/// @class InvalidPathError
/// @brief Ошибка неверного пути в реестре
class InvalidPathError : public RegistryException {
 public:
  explicit InvalidPathError(const std::string& path)
      : RegistryException(fmt::format("Неверный путь в реестре: {}", path)) {}
};

}
