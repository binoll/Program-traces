#include "file_reader.hpp"
#include <sstream>
#include "../exceptions/file_reader_exceptions.hpp"

FileReader::FileReader(std::string path)
    : file_path_(std::move(path)), handle_(std::make_unique<FileHandle>()) {}

void FileReader::open(std::ios::openmode mode) const {
  std::lock_guard<std::mutex> lock(handle_->mtx);

  if (handle_->stream.is_open())
    return;

  handle_->stream.open(file_path_, mode);
  if (!handle_->stream.is_open()) {
    throw FileOpenException(file_path_, errno);
  }
  handle_->last_pos = 0;
}

std::string FileReader::read() const {
  std::lock_guard<std::mutex> lock(handle_->mtx);

  safe_io_operation(
      [this] {
        handle_->stream.seekg(0);
        handle_->last_pos = 0;
      },
      "seek to start");

  std::stringstream buffer;
  safe_io_operation(
      [&] {
        buffer << handle_->stream.rdbuf();
        handle_->last_pos = handle_->stream.tellg();
      },
      "read full content");

  return buffer.str();
}

std::string FileReader::read_chunk(std::streamsize size) const {
  std::lock_guard<std::mutex> lock(handle_->mtx);

  std::string chunk(size, '\0');
  safe_io_operation(
      [&] {
        handle_->stream.read(&chunk[0], size);
        chunk.resize(handle_->stream.gcount());
        handle_->last_pos = handle_->stream.tellg();
      },
      "read chunk");

  return chunk;
}

void FileReader::set_pos(std::streampos pos) const {
  std::lock_guard<std::mutex> lock(handle_->mtx);

  safe_io_operation(
      [&] {
        handle_->stream.seekg(pos);
        handle_->last_pos = pos;
      },
      "set position");
}

std::streampos FileReader::get_pos() const {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  return handle_->last_pos;
}

void FileReader::close() const noexcept {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  if (handle_->stream.is_open()) {
    handle_->stream.close();
  }
}

bool FileReader::is_open() const noexcept {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  return handle_->stream.is_open();
}

const std::string& FileReader::path() const noexcept {
  return file_path_;
}

FileReader::~FileReader() noexcept {
  close();
}

void FileReader::check_stream_state(const std::ifstream& stream,
                                    const std::string& operation) const {
  if (!stream.is_open()) {
    throw FileStateException(operation, EBADF);
  }
  if (stream.fail() && !stream.eof()) {
    throw FileReadException(file_path_, errno);
  }
}

void FileReader::safe_io_operation(std::function<void()> op,
                                   const std::string& context) const {
  try {
    check_stream_state(handle_->stream, context);
    op();
    check_stream_state(handle_->stream, context);
  } catch (const FileException&) {
    handle_->stream.clear();
    handle_->stream.seekg(handle_->last_pos);
    throw;
  }
}
