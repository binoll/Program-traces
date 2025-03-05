#pragma once

#include <stdexcept>
#include <string>
#include <system_error>

class FileException : public std::runtime_error {
 public:
  FileException(const std::string& operation, const std::string& path,
                int error_code)
      : std::runtime_error("File operation '" + operation + "' failed for '" +
                           path + "': " + std::to_string(error_code) + " - " +
                           std::system_category().message(error_code)),
        error_code_(error_code) {}

  [[nodiscard]] int code() const noexcept { return error_code_; }

 private:
  int error_code_;
};

class FileOpenException : public FileException {
 public:
  FileOpenException(const std::string& path, int error_code)
      : FileException("open", path, error_code) {}
};

class FileReadException : public FileException {
 public:
  FileReadException(const std::string& path, int error_code)
      : FileException("read", path, error_code) {}
};

class FileSeekException : public FileException {
 public:
  FileSeekException(const std::string& path, int error_code)
      : FileException("seek", path, error_code) {}
};

class FileStateException : public FileException {
 public:
  FileStateException(const std::string& path, int error_code)
      : FileException("state_check", path, error_code) {}
};