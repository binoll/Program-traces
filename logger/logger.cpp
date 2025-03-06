#include "logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include "../constants.hpp"
#include "logger_exceptions.hpp"

/**
* @brief Вспомогательная функция для создания директорий
* @param path Путь к директории
* @throws std::filesystem::filesystem_error При ошибках создания
*/
void create_directory_if_not_exists(const std::string& path) {
  const std::filesystem::path dir(path);
  if (!std::filesystem::exists(dir)) {
    std::filesystem::create_directories(dir);
  }
}

std::shared_ptr<spdlog::logger> GlobalLogger::logger_ = nullptr;

void GlobalLogger::initialize() {
  try {
    constexpr size_t MAX_SIZE = 5 * SizeConstants::MB;
    constexpr size_t MAX_FILES = 5;

    create_directory_if_not_exists("logs");

    logger_ = spdlog::rotating_logger_mt("global", "logs/app.log", MAX_SIZE,
                                         MAX_FILES);
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
