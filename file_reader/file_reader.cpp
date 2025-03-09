#include "file_reader.hpp"
#include "../logger/logger.hpp"
#include "file_reader_exceptions.hpp"

FileReader::FileReader(const fs::path& path, std::ios::openmode mode)
    : file_path_(path), handle_(std::make_unique<FileHandle>()) {
  handle_->stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  handle_->stream.open(file_path_, mode);
  handle_->last_modified = fs::last_write_time(file_path_);
  start_monitoring();
}

FileReader::~FileReader() {
  stop_monitoring();
  if (handle_->stream.is_open()) {
    handle_->stream.close();
  }
}

FileReader::FileReader(FileReader&& other) noexcept
    : file_path_(std::move(other.file_path_)),
      handle_(std::move(other.handle_)),
      cache_(std::move(other.cache_)),
      max_cache_blocks_(other.max_cache_blocks_),
      callback_id_(other.callback_id_.load()) {
  other.stop_monitoring();
  start_monitoring();
}

FileReader& FileReader::operator=(FileReader&& other) noexcept {
  if (this != &other) {
    stop_monitoring();
    file_path_ = std::move(other.file_path_);
    handle_ = std::move(other.handle_);
    cache_ = std::move(other.cache_);
    max_cache_blocks_ = other.max_cache_blocks_;
    callback_id_.store(other.callback_id_.load());
    other.stop_monitoring();
    start_monitoring();
  }
  return *this;
}

std::vector<char> FileReader::read() const {
  std::unique_lock<std::timed_mutex> lock(handle_->mtx);
  handle_->stream.seekg(0, std::ios::end);
  const auto size = handle_->stream.tellg();
  handle_->stream.seekg(0, std::ios::beg);

  std::vector<char> content(static_cast<size_t>(size));
  handle_->stream.read(content.data(), size);

  if (handle_->stream.fail() && !handle_->stream.eof()) {
    throw std::runtime_error("Failed to read file: " + file_path_.string());
  }
  return content;
}

std::streamsize FileReader::file_size(std::chrono::milliseconds timeout) const {
  std::unique_lock<std::timed_mutex> lock(handle_->mtx, std::defer_lock);

  if (timeout.count() > 0 && !lock.try_lock_for(timeout)) {
    throw TimeoutException("File size operation timed out");
  }
  lock.lock();

  if (handle_->cached_size == -1) {
    const auto current_pos = handle_->stream.tellg();
    handle_->stream.seekg(0, std::ios::end);
    handle_->cached_size = handle_->stream.tellg();
    handle_->stream.seekg(current_pos);
  }
  return handle_->cached_size;
}

std::future<std::vector<char>> FileReader::async_read() const {
  return std::async(std::launch::async, [this]() {
    std::unique_lock<std::timed_mutex> lock(handle_->mtx);
    return read();
  });
}

void FileReader::set_cache_size(size_t max_blocks) {
  std::lock_guard lock(cache_mutex_);
  max_cache_blocks_ = max_blocks;
  check_cache_expiration();
}

size_t FileReader::subscribe_changes(
    std::function<void(const fs::path&)> callback) {
  std::lock_guard lock(callback_mutex_);
  const size_t id = ++callback_id_;
  callbacks_.emplace(id, std::move(callback));
  return id;
}

void FileReader::unsubscribe_changes(size_t token) {
  std::lock_guard lock(callback_mutex_);
  callbacks_.erase(token);
}

void FileReader::start_monitoring() {
  if (monitoring_active_)
    return;
  monitoring_active_ = true;
  monitoring_thread_ = std::thread(&FileReader::monitoring_loop, this);
}

void FileReader::stop_monitoring() {
  monitoring_active_ = false;
  if (monitoring_thread_.joinable()) {
    monitoring_thread_.join();
  }
}

void FileReader::monitoring_loop() {
  while (monitoring_active_) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    try {
      auto new_time = fs::last_write_time(file_path_);
      if (new_time != handle_->last_modified) {
        handle_->last_modified = new_time;
        handle_->cached_size = -1;

        std::shared_lock lock(callback_mutex_);
        for (const auto& [id, cb] : callbacks_) {
          cb(file_path_);
        }
      }
    } catch (const std::exception& e) {
      LOGGER->error("Monitoring error: {}", e.what());
    }
  }
}

void FileReader::check_cache_expiration() const {
  const auto now = std::chrono::steady_clock::now();
  const auto expiration_time = std::chrono::minutes(5);

  cache_.remove_if([&](const CacheBlock& block) {
    return (now - block.timestamp) > expiration_time;
  });

  while (cache_.size() > max_cache_blocks_) {
    cache_.pop_back();
  }
}
