#pragma once
#include <spdlog/spdlog.h>
#include <memory>

/**
 * @brief Глобальный логгер для всего проекта
 * @details Обеспечивает:
 * - Единую точку инициализации
 * - Потокобезопасность
 * - Ротацию логов
 */
class GlobalLogger {
 public:
  /// @brief Получить экземпляр логгера
  static std::shared_ptr<spdlog::logger> get();

 private:
  /// @brief Инициализация логгера
  static void initialize();

  static std::shared_ptr<spdlog::logger> logger_;
};

/// @brief Макрос для удобного доступа к логгеру
#define LOGGER GlobalLogger::get()
