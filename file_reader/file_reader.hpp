#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "../logger/logger.hpp"

/**
 * @defgroup filereader File Reader Module
 * @brief Потокобезопасное чтение файлов с обработкой ошибок
 */

/**
 * @ingroup filereader
 * @brief Внутреннее представление открытого файла
 * 
 * Инкапсулирует файловый поток, мьютекс для синхронизации
 * и текущую позицию чтения.
 */
struct FileHandle {
  std::ifstream stream;    ///< Бинарный поток ввода
  mutable std::mutex mtx;  ///< Межпоточная синхронизация
  std::streampos pos = 0;  ///< Текущая позиция в файле
};

/**
 * @ingroup filereader
 * @brief Основной класс для потокобезопасных файловых операций
 * 
 * @details Обеспечивает:
 * - Атомарные операции чтения
 * - Работу с бинарными данными
 * - Консистентное состояние потока
 * - Детализированные исключения
 * 
 * @warning Не поддерживает операции записи
 */
class FileReader {
 public:
  /**
   * @brief Конструктор с открытием файла
   * @param path Абсолютный или относительный путь к файлу
   * @param mode Флаги открытия файла (по умолчанию: бинарный режим)
   * @throws FileOpenException При ошибке открытия файла
   * 
   * @code
   * FileReader reader("data.bin", std::ios::in | std::ios::ate);
   * @endcode
   */
  explicit FileReader(std::string path,
                      std::ios::openmode mode = std::ios::in |
                                                std::ios::binary);

  ~FileReader();

  FileReader(const FileReader&) = delete;

  FileReader& operator=(const FileReader&) = delete;

  /**
   * @brief Чтение всего содержимого файла
   * @return Вектор байтов с данными файла
   * @throws FileReadException При ошибках ввода
   * 
   * @note Сбрасывает позицию чтения в начало файла
   */
  [[nodiscard]] std::vector<char> read() const;

  /**
   * @brief Чтение блока данных фиксированного размера
   * @param size Требуемое количество байтов
   * @return Фактически прочитанные данные (может быть меньше size)
   * @throws FileReadException При ошибках чтения
   */
  [[nodiscard]] std::vector<char> read_chunk(std::streamsize size) const;

  /**
   * @brief Изменение позиции чтения
   * @param pos Новая абсолютная позиция (от начала файла)
   * @throws FileSeekException При ошибках позиционирования
   */
  void seek(std::streampos pos) const;

  /// @brief Текущая позиция в файле
  [[nodiscard]] std::streampos tell() const noexcept;

  /// @brief Путь к обрабатываемому файлу
  [[nodiscard]] const std::string& path() const noexcept { return file_path_; }

  /// @brief Состояние файлового потока
  [[nodiscard]] bool is_open() const noexcept;

 private:
  void validate_stream() const;

  void handle_io_error(const std::string& operation, int err) const;

  /**
   * @brief Чтение бинарных данных заданного типа
   * @tparam type Тип данных (целочисленные типы)
   * @param offset Смещение от начала файла
   * @return Прочитанное значение
   * @throws FileReadException При недостатке данных или ошибках чтения
   * 
   * @note Автоматически обновляет позицию чтения
   */
  template <typename type>
  type read_binary_value(uint32_t offset) const;

  std::string file_path_;               ///< Кешированный путь к файлу
  std::unique_ptr<FileHandle> handle_;  ///< Ресурсы файла
};
