#pragma once

#include <stdexcept>
#include <string>

namespace WindowsVersion {

class OSDetectionException : public std::runtime_error {
public:
  explicit OSDetectionException(const std::string& message)
      : std::runtime_error("Ошибка обнаружения операционной системы: " + message) {}
};

class RegistryKeyNotFoundException : public OSDetectionException {
public:
  explicit RegistryKeyNotFoundException(const std::string& key_path)
      : OSDetectionException("Раздел реестра не найден: " + key_path) {}
};

}
