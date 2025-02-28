#pragma once

#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

struct FileHandle {
  std::ifstream stream;
  std::mutex mtx;
  std::streampos last_pos = 0;
};

class FileReader {
 public:
  explicit FileReader(std::string path);

  void open(std::ios::openmode) const;
  [[nodiscard]] std::string read() const;
  [[nodiscard]] std::string read_chunk(std::streamsize) const;
  void set_pos(std::streampos pos) const;
  [[nodiscard]] std::streampos get_pos() const;
  void close() const noexcept;

  [[nodiscard]] bool is_open() const noexcept;
  [[nodiscard]] const std::string& path() const noexcept;

  ~FileReader() noexcept;

 private:
  void check_stream_state(const std::ifstream&, const std::string&) const;
  void safe_io_operation(std::function<void()>, const std::string&) const;

  std::string file_path_;
  std::unique_ptr<FileHandle> handle_;
};
