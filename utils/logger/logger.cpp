#include "logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include "logger_exception.hpp"

std::string GlobalLogger::logPath_ = "logs/app.log";
std::shared_ptr<spdlog::logger> GlobalLogger::logger_ = nullptr;

void GlobalLogger::setLogPath(const std::string& path) {
  if (logger_) {
    throw std::logic_error(
        "Логгер уже инициализирован. Установите путь к логу до первого "
        "использования.");
  }
  logPath_ = path;
}

void create_directory_if_not_exists(const std::string& file_path) {
  const std::filesystem::path path(file_path);
  auto parent_dir = path.parent_path();

  if (!parent_dir.empty() && !std::filesystem::exists(parent_dir)) {
    std::filesystem::create_directories(parent_dir);
  }
}

void GlobalLogger::initialize() {
  try {
    constexpr size_t MAX_SIZE = 5 * 1024 * 1024;
    constexpr size_t MAX_FILES = 5;

    create_directory_if_not_exists(logPath_);

    logger_ =
        spdlog::rotating_logger_mt("global", logPath_, MAX_SIZE, MAX_FILES);
    logger_->set_pattern("[%Y-%m-%d %T.%e] [%l] %v");
    logger_->set_level(spdlog::level::debug);
  } catch (const spdlog::spdlog_ex& ex) {
    throw LoggerInitException("Ошибка SPDLOG: " + std::string(ex.what()));
  } catch (const std::filesystem::filesystem_error& ex) {
    throw LoggerInitException("Ошибка файловой системы: " +
                              std::string(ex.what()));
  } catch (const std::exception& ex) {
    throw LoggerInitException("Общая ошибка: " + std::string(ex.what()));
  }
}

std::shared_ptr<spdlog::logger> GlobalLogger::get() {
  if (!logger_) {
    initialize();
  }

  return logger_;
}
