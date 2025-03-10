#include "file_reader.hpp"
#include "../logger/logger.hpp"
#include "file_reader_exceptions.hpp"

FileReader::FileReader(const fs::path& path, std::ios::openmode mode)
    : file_path_(path), handle_(std::make_unique<FileHandle>()) {

  LOGGER->debug("Инициализация FileReader для файла: {}", path.string());

  try {
    handle_->stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    handle_->stream.open(file_path_, mode);
    handle_->last_modified = fs::last_write_time(file_path_);

    LOGGER->trace("Файл успешно открыт. Режим: {:#x}", mode);
    LOGGER->trace("Время последней модификации: {}",
                  std::chrono::system_clock::to_time_t(
                      std::chrono::file_clock::to_sys(handle_->last_modified)));

    start_monitoring();
  } catch (const std::exception& e) {
    LOGGER->critical("Ошибка при открытии файла: {}", e.what());
    throw FileOpenException(path.string(), errno);
  }
}

FileReader::~FileReader() {
  LOGGER->debug("Деструктор FileReader для файла: {}", file_path_.string());
  stop_monitoring();

  if (handle_->stream.is_open()) {
    LOGGER->trace("Закрытие файлового потока");
    handle_->stream.close();
  }
}

FileReader::FileReader(FileReader&& other) noexcept
    : file_path_(std::move(other.file_path_)),
      handle_(std::move(other.handle_)),
      cache_(std::move(other.cache_)),
      max_cache_blocks_(other.max_cache_blocks_),
      callback_id_(other.callback_id_.load()) {

  LOGGER->debug("Перемещение FileReader для файла: {}", file_path_.string());
  other.stop_monitoring();
  start_monitoring();
}

FileReader& FileReader::operator=(FileReader&& other) noexcept {
  if (this != &other) {
    LOGGER->debug("Перемещение присваиванием для файла: {}",
                  other.file_path_.string());

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
  LOGGER->trace("Начало операции чтения файла: {}", file_path_.string());

  std::unique_lock<std::timed_mutex> lock(handle_->mtx);
  LOGGER->debug("Блокировка мьютекса для чтения");

  try {
    handle_->stream.seekg(0, std::ios::end);
    const auto size = handle_->stream.tellg();
    handle_->stream.seekg(0, std::ios::beg);

    LOGGER->debug("Размер файла для чтения: {} байт", size);

    if (size <= 0) {
      LOGGER->debug("Попытка чтения пустого файла");
      return {};
    }

    std::vector<char> content(static_cast<size_t>(size));
    handle_->stream.read(content.data(), size);

    if (handle_->stream.fail() && !handle_->stream.eof()) {
      LOGGER->error("Ошибка чтения файла: {}", file_path_.string());
      throw FileReadException(file_path_.string(), "Ошибка потока чтения");
    }

    LOGGER->trace("Успешно прочитано {} байт", content.size());

    return content;
  } catch (const std::exception& e) {
    LOGGER->error("Ошибка при чтении файла: {}", e.what());
    throw;
  }
}

std::streamsize FileReader::file_size(std::chrono::milliseconds timeout) const {
  LOGGER->debug("Запрос размера файла (таймаут: {} мс)", timeout.count());

  std::unique_lock<std::timed_mutex> lock(handle_->mtx, std::defer_lock);

  if (timeout.count() > 0) {
    LOGGER->trace("Попытка блокировки с таймаутом");
    if (!lock.try_lock_for(timeout)) {
      LOGGER->warn("Таймаут получения размера файла: {}", file_path_.string());
      throw TimeoutException("Таймаут операции получения размера");
    }
  } else {
    LOGGER->trace("Блокировка без таймаута");
    lock.lock();
  }

  try {
    if (handle_->cached_size == -1) {
      LOGGER->debug("Обновление кешированного размера файла");
      const auto current_pos = handle_->stream.tellg();
      handle_->stream.seekg(0, std::ios::end);
      handle_->cached_size = handle_->stream.tellg();
      handle_->stream.seekg(current_pos);
      LOGGER->trace("Новый размер в кеше: {} байт", handle_->cached_size);
    }

    return handle_->cached_size;
  } catch (const std::exception& e) {
    LOGGER->error("Ошибка при определении размера: {}", e.what());
    throw;
  }
}

std::future<std::vector<char>> FileReader::async_read() const {
  LOGGER->debug("Запуск асинхронного чтения файла: {}", file_path_.string());

  return std::async(std::launch::async, [this]() {
    LOGGER->trace("Начало асинхронной операции чтения");
    try {
      auto result = read();
      LOGGER->debug("Асинхронное чтение завершено успешно");
      return result;
    } catch (...) {
      LOGGER->error("Ошибка в асинхронном чтении");
      throw;
    }
  });
}

void FileReader::set_cache_size(size_t max_blocks) {
  LOGGER->debug("Изменение размера кеша: {} -> {}", max_cache_blocks_,
                max_blocks);

  std::lock_guard lock(cache_mutex_);
  max_cache_blocks_ = max_blocks;
  check_cache_expiration();
}

size_t FileReader::subscribe_changes(
    std::function<void(const fs::path&)> callback) {
  LOGGER->debug("Регистрация нового подписчика");

  std::lock_guard lock(callback_mutex_);
  const size_t id = ++callback_id_;
  callbacks_.emplace(id, std::move(callback));

  LOGGER->trace("Выдан идентификатор подписки: {}", id);
  return id;
}

void FileReader::unsubscribe_changes(size_t token) {
  LOGGER->debug("Отмена подписки: {}", token);

  std::lock_guard lock(callback_mutex_);
  if (callbacks_.erase(token) > 0) {
    LOGGER->trace("Подписка {} успешно удалена", token);
  } else {
    LOGGER->warn("Попытка отмены несуществующей подписки: {}", token);
  }
}

void FileReader::start_monitoring() {
  if (monitoring_active_) {
    LOGGER->trace("Мониторинг уже активен");
    return;
  }

  LOGGER->debug("Запуск мониторинга изменений");
  monitoring_active_ = true;
  monitoring_thread_ = std::thread(&FileReader::monitoring_loop, this);
}

void FileReader::stop_monitoring() {
  if (!monitoring_active_)
    return;

  LOGGER->debug("Остановка мониторинга");
  monitoring_active_ = false;
  if (monitoring_thread_.joinable()) {
    monitoring_thread_.join();
    LOGGER->trace("Поток мониторинга остановлен");
  }
}

void FileReader::monitoring_loop() {
  LOGGER->debug("Поток мониторинга запущен для файла: {}", file_path_.string());

  while (monitoring_active_) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOGGER->trace("Проверка изменений файла");

    try {
      auto new_time = fs::last_write_time(file_path_);
      if (new_time != handle_->last_modified) {
        LOGGER->debug("Обнаружено изменение файла!");
        handle_->last_modified = new_time;
        handle_->cached_size = -1;

        {
          std::shared_lock lock(callback_mutex_);
          LOGGER->debug("Уведомление {} подписчиков", callbacks_.size());
          for (const auto& [id, cb] : callbacks_) {
            cb(file_path_);
          }
        }
      }
    } catch (const std::exception& e) {
      LOGGER->error("Ошибка мониторинга: {}", e.what());
    }
  }
  LOGGER->debug("Цикл мониторинга завершен");
}

void FileReader::check_cache_expiration() const {
  LOGGER->trace("Проверка устаревших кеш-блоков");

  const auto now = std::chrono::steady_clock::now();
  const auto expiration_time = std::chrono::minutes(5);

  auto removed = cache_.remove_if([&](const CacheBlock& block) {
    return (now - block.timestamp) > expiration_time;
  });

  LOGGER->debug("Удалено устаревших блоков: {}", removed);

  while (cache_.size() > max_cache_blocks_) {
    LOGGER->trace("Удаление старого блока (текущий размер: {})", cache_.size());
    cache_.pop_back();
  }
}
