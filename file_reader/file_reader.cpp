#include "file_reader.hpp"
#include <algorithm>
#include <iterator>
#include <utility>
#include "file_reader_exceptions.hpp"

FileReader::FileReader(std::string path, std::ios::openmode mode)
    : file_path_(std::move(path)), handle_(std::make_unique<Handle>()) {
  std::lock_guard<std::mutex> lock(handle_->mtx);

  handle_->stream.open(file_path_, mode);
  if (!handle_->stream.is_open()) {
    throw FileOpenException(file_path_, errno);
  }

  handle_->pos = handle_->stream.tellg();
}

FileReader::~FileReader() {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  if (handle_->stream.is_open()) {
    handle_->stream.close();
  }
}

std::vector<char> FileReader::read() const {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  validate_stream();

  try {
    handle_->stream.seekg(0);
    std::vector<char> content(std::istreambuf_iterator<char>(handle_->stream),
                              {});

    handle_->pos = handle_->stream.tellg();

    if (handle_->stream.fail() && !handle_->stream.eof()) {
      handle_io_error("read", errno);
    }

    return content;
  } catch (const std::ios_base::failure&) {
    handle_io_error("read", errno);
  }
  return {};
}

std::vector<char> FileReader::read_chunk(std::streamsize size) const {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  validate_stream();

  std::vector<char> buffer(size, '\0');

  try {
    handle_->stream.read(buffer.data(), size);
    buffer.resize(handle_->stream.gcount());

    handle_->pos = handle_->stream.tellg();

    if (handle_->stream.fail() && !handle_->stream.eof()) {
      handle_io_error("read_chunk", errno);
    }

    return buffer;
  } catch (const std::ios_base::failure&) {
    handle_io_error("read_chunk", errno);
  }
  return {};
}

void FileReader::seek(std::streampos pos) const {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  validate_stream();

  try {
    handle_->stream.seekg(pos);
    handle_->pos = handle_->stream.tellg();

    if (handle_->stream.fail()) {
      handle_io_error("seek", errno);
    }
  } catch (const std::ios_base::failure&) {
    handle_io_error("seek", errno);
  }
}

std::streampos FileReader::tell() const {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  return handle_->pos;
}

const std::string& FileReader::path() const noexcept {
  return file_path_;
}

bool FileReader::is_open() const noexcept {
  std::lock_guard<std::mutex> lock(handle_->mtx);
  return handle_->stream.is_open();
}

void FileReader::validate_stream() const {
  if (!handle_->stream.is_open()) {
    throw FileStateException(file_path_, EBADF);
  }
}

void FileReader::handle_io_error(const std::string& operation, int err) const {
  handle_->stream.clear();
  handle_->stream.seekg(handle_->pos);

  if (operation == "read" || operation == "read_chunk") {
    throw FileReadException(file_path_, err);
  }
  if (operation == "seek") {
    throw FileSeekException(file_path_, err);
  }

  throw FileException(operation, file_path_, err);
}