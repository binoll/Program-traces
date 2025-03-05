#include "logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>

std::shared_ptr<spdlog::logger> GlobalLogger::logger_ = nullptr;

void GlobalLogger::initialize() {
  try {
    // Создание ротирующего логгера (5 файлов по 5 МБ каждый)
    constexpr size_t MAX_SIZE = 5 * 1024 * 1024;  // 5 MB
    constexpr size_t MAX_FILES = 5;

    logger_ = spdlog::rotating_logger_mt("global", "logs/app.log", MAX_SIZE,
                                         MAX_FILES);

    // Настройка формата: [дата время] [уровень] сообщение
    logger_->set_pattern("[%Y-%m-%d %T.%e] [%l] %v");

    // Уровень логирования по умолчанию
    logger_->set_level(spdlog::level::debug);

  } catch (const spdlog::spdlog_ex& ex) {
    throw std::runtime_error("Logger init failed: " + std::string(ex.what()));
  }
}

std::shared_ptr<spdlog::logger> GlobalLogger::get() {
  if (!logger_) {
    initialize();
  }
  return logger_;
}