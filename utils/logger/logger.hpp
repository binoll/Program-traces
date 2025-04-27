#pragma once

#include <spdlog/spdlog.h>

#include <memory>

/**
 * @defgroup logger Система логирования
 * @brief Модуль для управления глобальным логированием приложения
 */

/**
 * @ingroup logger
 * @brief Глобальный потокобезопасный логгер с ротацией файлов
 * @details Обеспечивает централизованную конфигурацию логирования для всего
 * приложения. Автоматически создает директорию для логов при первом
 * использовании.
 */
class GlobalLogger {
 public:
  /**
   * @brief Установка пути к лог-файлу
   * @param path Полный путь к файлу лога (включая имя файла)
   * @throws std::logic_error При попытке изменить путь после инициализации
   * логгера
   * @note Должен быть вызван до первого использования логгера
   */
  static void setLogPath(const std::string& path);

  /**
   * @brief Получение экземпляра логгера
   * @return std::shared_ptr<spdlog::logger> Общий указатель на логгер
   * @throws LoggerInitException При ошибках инициализации
   *
   * @note При первом вызове автоматически выполняет инициализацию системы
   * логирования
   */
  static std::shared_ptr<spdlog::logger> get();

 private:
  /**
   * @brief Инициализация системы логирования
   * @details Выполняет:
   * - Создание директории для логов
   * - Настройку ротации лог-файлов
   * - Конфигурацию формата вывода
   *
   * @throws LoggerInitException При любых ошибках инициализации
   */
  static void initialize();

  /// @brief Путь к лог-файлу (по умолчанию: "logs/app.log")
  static std::string log_path_;

  /// @brief Статический экземпляр логгера
  static std::shared_ptr<spdlog::logger> logger_;
};

#define LOGGER GlobalLogger::get()
