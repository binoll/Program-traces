#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

struct Handle {
  std::ifstream stream;
  mutable std::mutex mtx;
  std::streampos pos = 0;
};

class FileReader {
 public:
  explicit FileReader(std::string path, std::ios::openmode);
  ~FileReader();

  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;

  [[nodiscard]] std::vector<char> read() const;
  [[nodiscard]] std::vector<char> read_chunk(std::streamsize) const;
  void seek(std::streampos) const;
  [[nodiscard]] std::streampos tell() const;

  [[nodiscard]] const std::string& path() const noexcept;
  [[nodiscard]] bool is_open() const noexcept;

 private:
  void validate_stream() const;
  void handle_io_error(const std::string&, int) const;

  std::string file_path_;
  std::unique_ptr<Handle> handle_;
};
