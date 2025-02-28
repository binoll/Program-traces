#pragma once

#include <stdexcept>
#include <string>
#include <system_error>

class FileException : public std::runtime_error {
 public:
  FileException(const std::string& msg, int error_code)
      : std::runtime_error(msg + " (" + std::to_string(error_code) + ": " +
                           std::system_category().message(error_code) + ")"),
        error_code_(error_code) {}

  [[nodiscard]] int code() const noexcept { return error_code_; }

 private:
  int error_code_;
};

class FileOpenException : public FileException {
 public:
  explicit FileOpenException(const std::string& path, int error_code)
      : FileException("Failed to open file: " + path, error_code) {}
};

class FileReadException : public FileException {
 public:
  explicit FileReadException(const std::string& path, int error_code)
      : FileException("Read error in file: " + path, error_code) {}
};

class FileSeekException : public FileException {
 public:
  explicit FileSeekException(const std::string& path, int error_code)
      : FileException("Seek error in file: " + path, error_code) {}
};

class FileStateException : public FileException {
 public:
  explicit FileStateException(const std::string& operation, int error_code)
      : FileException("Invalid state for operation: " + operation, error_code) {
  }
};
