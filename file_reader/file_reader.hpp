#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "../logger/logger.hpp"

/**
 * @defgroup filereader Модуль файлового ввода
 * @brief Потокобезопасные операции чтения файлов
 */

/**
 * @ingroup filereader
 * @brief Внутреннее состояние открытого файла
 * 
 * Содержит файловый поток, мьютекс для синхронизации
 * и текущую позицию чтения.
 */
struct FileHandle {
  std::ifstream stream;    ///< Файловый поток ввода
  mutable std::mutex mtx;  ///< Механизм синхронизации потоков
  std::streampos pos = 0;  ///< Текущее смещение в файле
};

/**
 * @ingroup filereader
 * @brief Класс для потокобезопасного чтения файлов
 * 
 * @details Реализует:
 * - Атомарные операции чтения
 * - Бинарный и текстовый режимы работы
 * - Механизм обработки ошибок
 * - Управление позицией чтения
 */
class FileReader {
 public:
  /**
   * @brief Конструктор с открытием файла
   * @param path Путь к целевому файлу
   * @param mode Режим открытия файла (по умолчанию: бинарный)
   * @throws FileOpenException При ошибке открытия
   */
  explicit FileReader(std::string path,
                      std::ios::openmode mode = std::ios::in |
                                                std::ios::binary);

  ~FileReader();

  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;

  /**
   * @brief Чтение всего содержимого файла
   * @return Вектор с бинарными данными файла
   * @throws FileReadException При ошибке чтения
   */
  [[nodiscard]] std::vector<char> read() const;

  /**
   * @brief Чтение блока данных
   * @param size Требуемый размер блока
   * @return Прочитанные данные (фактический размер может быть меньше)
   * @throws FileReadException При ошибке чтения
   */
  [[nodiscard]] std::vector<char> read_chunk(std::streamsize size) const;

  /**
   * @brief Установка позиции чтения
   * @param pos Абсолютное смещение от начала файла
   * @throws FileSeekException При ошибке позиционирования
   */
  void seek(std::streampos pos) const;

  /**
   * @brief Получение текущей позиции
   * @return Текущее смещение в файле
   */
  [[nodiscard]] std::streampos tell() const noexcept;

  /**
   * @brief Получение пути к файлу
   * @return Путь, переданный при создании объекта
   */
  [[nodiscard]] const std::string& path() const noexcept { return file_path_; }

  /**
   * @brief Проверка состояния потока
   * @return true если файл открыт, false в противном случае
   */
  [[nodiscard]] bool is_open() const noexcept;

  /**
   * @brief Чтение бинарных данных заданного типа
   * @return Прочитанное значение
   * @throws FileReadException При ошибке чтения
   */
  template <typename type>
  type read_binary_value(uint32_t offset) const;

 private:
  /// @name Внутренние методы
  /// @{

  /**
   * @brief Проверка валидности файлового потока
   * @throws FileStateException При невалидном состоянии
   */
  void validate_stream() const;

  /**
   * @brief Обработчик ошибок ввода-вывода
   * @param operation Имя операции, вызвавшей ошибку
   * @param err Код системной ошибки
   */
  void handle_io_error(const std::string& operation, int err) const;

  /// @}

  std::string file_path_;               ///< Путь к обрабатываемому файлу
  std::unique_ptr<FileHandle> handle_;  ///< Управление файловым дескриптором
};
