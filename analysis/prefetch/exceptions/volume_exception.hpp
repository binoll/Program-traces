/**
 * @file volume_exception.hpp
 * @brief Исключения, связанные с работой с томами.
 * @details Этот файл содержит базовые классы исключений для обработки ошибок, связанных с работой с томами и устройствами.
*/

#pragma once

#include <stdexcept>
#include <string>

namespace PrefetchAnalysis {

/**
 * @brief Базовый класс для всех исключений, связанных с ошибками работы с томами.
 * @details Этот класс является базой для всех исключений, которые происходят в контексте работы с томами и устройствами.
*/
class VolumeException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор базового исключения.
   * @param message Описание ошибки в кодировке UTF-8.
   * @details Конструктор передает сообщение об ошибке в базовый класс std::runtime_error.
  */
  explicit VolumeException(const std::string& message)
      : std::runtime_error(message) {}

  /**
   * @brief Деструктор.
   * @details Переопределенный деструктор для корректной очистки.
  */
  ~VolumeException() noexcept override = default;
};

/**
 * @brief Исключение для некорректного пути к устройству.
 * @details Генерируется при обнаружении невалидного пути в формате NT (например, "\Device\HarddiskVolumeX").
*/
class InvalidDevicePathException : public VolumeException {
 public:
  /**
   * @brief Конструктор исключения для некорректного пути.
   * @param path Некорректный путь к устройству.
   * @details Конструктор формирует сообщение об ошибке, добавляя некорректный путь к устройству.
  */
  explicit InvalidDevicePathException(const std::string& path)
      : VolumeException("Недопустимый путь к устройству: " + path) {}
};

}
