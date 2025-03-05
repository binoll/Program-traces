#pragma once

#include <spdlog/spdlog.h>
#include <memory>

/**
 * @brief Глобальный логгер для всего проекта.
 * @details Инициализируется при первом использовании.
 */
class GlobalLogger {
public:
  /// @brief Получить экземпляр логгера
  static std::shared_ptr<spdlog::logger> get() {
    static auto logger = create_logger();
    return logger;
  }

private:
  /// @brief Создание логгера
  static std::shared_ptr<spdlog::logger> create_logger() {
    auto logger = spdlog::basic_logger_mt("global", "logs/global.log");
    logger->set_level(spdlog::level::debug); // Уровень логирования
    logger->set_pattern("[%Y-%m-%d %T.%e] [%l] %v"); // Формат сообщений
    return logger;
  }
};

/// @brief Макрос для удобного доступа к логгеру
#define LOGGER GlobalLogger::get()
