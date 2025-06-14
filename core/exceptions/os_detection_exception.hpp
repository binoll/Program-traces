/// @file os_detection_exception.hpp
/// @brief Исключения, возникающее при ошибках определения операционной системы

#pragma once

#include <stdexcept>
#include <string>

namespace WindowsVersion {

/// @brief Исключение, возникающее при ошибках определения операционной системы
class OSDetectionException : public std::runtime_error {
 public:
  /// @brief Конструктор исключения
  /// @param message Сообщение об ошибке
  explicit OSDetectionException(const std::string& message)
      : std::runtime_error(message) {}
};

}
