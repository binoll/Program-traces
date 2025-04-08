/**
* @file prefetch_parser.hpp
 * @brief Заголовочный файл парсера Prefetch-файлов
 */

#pragma once

#include <libscca.h>
#include <memory>
#include <stdexcept>
#include "prefetch_info.hpp"

namespace PrefetchAnalysis {

/**
 * @brief Базовый класс для обработки ошибок
 */
class ErrorHandler {
 public:
  virtual void handle(const std::string& context, libscca_error_t* error) = 0;
  virtual ~ErrorHandler() = default;
};

/**
 * @brief Реализация обработки ошибок через исключения
 */
class ExceptionErrorHandler : public ErrorHandler {
 public:
  void handle(const std::string& context, libscca_error_t* error) override {
    char buffer[256];
    if (error) {
      libscca_error_sprint(error, buffer, sizeof(buffer));
      throw std::runtime_error(context + ": " + buffer);
    }
    throw std::runtime_error(context);
  }
};

/**
 * @brief Основной класс парсера
 */
class Parser {
 public:
  Parser() {
    initialize();
  }

  explicit Parser(std::unique_ptr<ErrorHandler> error_handler)
      : error_handler_(std::move(error_handler)) {
    initialize();
  }

  ~Parser() {
    if (scca_file_) {
      libscca_file_free(&scca_file_, nullptr);
    }
  }

  PrefetchData parse(const std::string& path);

 private:
  void initialize();
  void parse_basic_info(PrefetchData& data);
  void parse_run_history(PrefetchData& data);
  void parse_volumes(PrefetchData& data);
  void parse_metrics(PrefetchData& data);
  time_t convert_filetime_safe(uint64_t filetime);
  std::string format_time_safe(time_t time);


  libscca_file_t* scca_file_ = nullptr;
  std::unique_ptr<ErrorHandler> error_handler_;
};

}  // namespace PrefetchAnalysis
