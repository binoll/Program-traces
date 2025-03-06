#include "file_reader.hpp"
#include <algorithm>
#include "file_reader_exceptions.hpp"

FileReader::FileReader(std::string path, std::ios::openmode mode)
    : file_path_(std::move(path)), handle_(std::make_unique<FileHandle>()) {
  LOGGER->debug("Инициализация FileReader: {}", file_path_);

  std::lock_guard lock(handle_->mtx);
  handle_->stream.open(file_path_, mode);

  if (!handle_->stream.is_open()) {
    throw FileOpenException(file_path_, errno);
  }

  handle_->pos = handle_->stream.tellg();
}

FileReader::~FileReader() {
  std::lock_guard lock(handle_->mtx);
  if (handle_->stream.is_open()) {
    LOGGER->debug("Закрытие файла: {}", file_path_);
    handle_->stream.close();
  }
}

std::vector<char> FileReader::read() const {
  std::lock_guard lock(handle_->mtx);
  LOGGER->trace("Чтение всего файла: {}", file_path_);

  validate_stream();
  handle_->stream.seekg(0);

  std::vector<char> content((std::istreambuf_iterator<char>(handle_->stream)),
                            std::istreambuf_iterator<char>());

  if (handle_->stream.fail() && !handle_->stream.eof()) {
    handle_io_error("read", errno);
  }

  handle_->pos = handle_->stream.tellg();

  return content;
}

std::vector<char> FileReader::read_chunk(std::streamsize size) const {
  std::lock_guard lock(handle_->mtx);
  LOGGER->trace("Чтение блока {} байт", size);

  validate_stream();
  std::vector<char> buffer(size, 0);

  handle_->stream.read(buffer.data(), size);
  buffer.resize(handle_->stream.gcount());

  if (handle_->stream.fail() && !handle_->stream.eof()) {
    handle_io_error("read_chunk", errno);
  }

  handle_->pos = handle_->stream.tellg();

  return buffer;
}

void FileReader::seek(std::streampos pos) const {
  std::lock_guard lock(handle_->mtx);
  LOGGER->trace("Установка позиции: {}", pos);

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
  if (!handle_->stream.is_open()) {
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

template <typename type>
type FileReader::read_binary_value(uint32_t offset) const {
  seek(offset);
  const auto data = read_chunk(sizeof(type));

  if (data.size() < sizeof(type)) {
    throw FileReadException(file_path_, "Недостаточно данных для типа " +
                                            std::string(typeid(type).name()));
  }

  return *reinterpret_cast<const type*>(data.data());
}

// Явные инстанциации шаблонов
template uint32_t FileReader::read_binary_value<uint32_t>(uint32_t) const;
template uint16_t FileReader::read_binary_value<uint16_t>(uint32_t) const;
