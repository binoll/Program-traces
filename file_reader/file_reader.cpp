#include "file_reader.hpp"
#include <algorithm>
#include "file_reader_exceptions.hpp"

FileReader::FileReader(std::string path, std::ios::openmode mode)
    : file_path_(std::move(path)), handle_(std::make_unique<FileHandle>()) {
  LOGGER->debug("Инициализация FileReader для '{}'", file_path_);

  std::lock_guard lock(handle_->mtx);
  handle_->stream.open(file_path_, mode);

  if (!handle_->stream.is_open()) {
    const int error_code = errno;
    throw FileOpenException(file_path_, error_code);
  }

  handle_->pos = handle_->stream.tellg();
}

FileReader::~FileReader() {
  std::lock_guard lock(handle_->mtx);
  if (handle_->stream.is_open()) {
    LOGGER->debug("Закрытие файла '{}'", file_path_);
    handle_->stream.close();
  }
}

std::vector<char> FileReader::read() const {
  std::lock_guard lock(handle_->mtx);
  LOGGER->trace("Полное чтение файла '{}'", file_path_);

  validate_stream();
  handle_->stream.seekg(0, std::ios::beg);

  std::vector<char> content;
  const auto start_pos = handle_->stream.tellg();
  handle_->stream.seekg(0, std::ios::end);
  const auto size = handle_->stream.tellg() - start_pos;
  handle_->stream.seekg(0, std::ios::beg);

  content.resize(static_cast<size_t>(size));
  handle_->stream.read(content.data(), size);

  if (handle_->stream.fail() && !handle_->stream.eof()) {
    handle_io_error("read", errno);
  }

  handle_->pos = handle_->stream.tellg();
  LOGGER->debug("Прочитано {} байт из '{}'", content.size(), file_path_);
  return content;
}

std::vector<char> FileReader::read_chunk(std::streamsize size) const {
  std::lock_guard lock(handle_->mtx);
  LOGGER->trace("Чтение блока {} байт из '{}'", size, file_path_);

  validate_stream();
  std::vector<char> buffer(size, 0);

  handle_->stream.read(buffer.data(), size);
  const auto bytes_read = handle_->stream.gcount();
  buffer.resize(static_cast<size_t>(bytes_read));

  if (handle_->stream.fail() && !handle_->stream.eof()) {
    handle_io_error("read_chunk", errno);
  }

  handle_->pos = handle_->stream.tellg();
  LOGGER->trace("Успешно прочитано {} байт", bytes_read);
  return buffer;
}

void FileReader::seek(std::streampos pos) const {
  std::lock_guard lock(handle_->mtx);
  LOGGER->trace("Переход на позицию {} в файле '{}'", pos, file_path_);

  validate_stream();
  handle_->stream.seekg(pos);

  if (handle_->stream.fail()) {
    handle_io_error("seek", errno);
  }

  handle_->pos = handle_->stream.tellg();
}

std::streampos FileReader::tell() const noexcept {
  std::lock_guard lock(handle_->mtx);
  return handle_->pos;
}

bool FileReader::is_open() const noexcept {
  std::lock_guard lock(handle_->mtx);
  return handle_->stream.is_open();
}

void FileReader::validate_stream() const {
  if (!handle_->stream) {
    throw FileStateException(file_path_, EBADF);
  }
}

void FileReader::handle_io_error(const std::string& operation, int err) const {
  handle_->stream.clear();
  handle_->stream.seekg(handle_->pos);

  if (operation == "read" || operation == "read_chunk") {
    throw FileReadException(file_path_, std::to_string(err));
  }
  if (operation == "seek") {
    throw FileSeekException(file_path_, err);
  }

  throw FileException(file_path_, "Неизвестная операция", err);
}

template <typename T>
T FileReader::read_binary_value(uint32_t offset) const {
  LOGGER->trace("Чтение типа {} по смещению 0x{:08X}", typeid(T).name(),
                offset);

  seek(offset);
  const auto data = read_chunk(sizeof(T));

  if (data.size() < sizeof(T)) {
    throw FileReadException(file_path_, "Недостаточно данных для типа " +
                                            std::string(typeid(T).name()));
  }

  T value;
  std::memcpy(&value, data.data(), sizeof(T));
  return value;
}

// Явные инстанциации для поддерживаемых типов
extern template uint32_t FileReader::read_binary_value<uint32_t>(
    uint32_t) const;
extern template uint16_t FileReader::read_binary_value<uint16_t>(
    uint32_t) const;
