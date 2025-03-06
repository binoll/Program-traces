#pragma once

#include <spdlog/spdlog.h>
#include <memory>

/**
 * @brief Система глобального логирования
 * @details Обеспечивает:
 * - Ротацию логов
 * - Потокобезопасность
 * - Единую точку конфигурации
 */
class GlobalLogger {
 public:
  /// @brief Получить экземпляр логгера
  static std::shared_ptr<spdlog::logger> get();

 private:
  /// @brief Инициализация системы логирования
  static void initialize();

  static std::shared_ptr<spdlog::logger> logger_;
};
