#pragma once

#include <stdexcept>
#include <string>

class RegistryException : public std::runtime_error {
public:
  explicit RegistryException(const std::string& msg)
      : std::runtime_error(msg) {}
};

class InvalidHiveFormat : public RegistryException {
public:
  explicit InvalidHiveFormat(const std::string& details)
      : RegistryException("Invalid registry hive format: " + details) {}
};

class KeyNotFound : public RegistryException {
public:
  explicit KeyNotFound(const std::string& key)
      : RegistryException("Registry key not found: " + key) {}
};

class ValueNotFound : public RegistryException {
public:
  explicit ValueNotFound(const std::string& value)
      : RegistryException("Registry value not found: " + value) {}
};

class DecodingError : public RegistryException {
public:
  explicit DecodingError(const std::string& details)
      : RegistryException("String decoding failed: " + details) {}
};
