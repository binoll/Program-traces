#include "logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include "logger_exceptions.hpp"

std::shared_ptr<spdlog::logger> GlobalLogger::logger_ = nullptr;

void GlobalLogger::initialize() {
  try {
    constexpr size_t MAX_SIZE = 5 * 1024 * 1024;
    constexpr size_t MAX_FILES = 5;

    logger_ = spdlog::rotating_logger_mt("global", "logs/app.log", MAX_SIZE,
                                         MAX_FILES);

    logger_->set_pattern("[%Y-%m-%d %T.%e] [%l] %v");
    logger_->set_level(spdlog::level::debug);

  } catch (const spdlog::spdlog_ex& ex) {
    throw LoggerInitException(ex.what());
  } catch (const std::exception& ex) {
    throw LoggerInitException(std::string("Ошибка: ") + ex.what());
  }
}

std::shared_ptr<spdlog::logger> GlobalLogger::get() {
  if (!logger_) {
    initialize();
  }

  return logger_;
}
