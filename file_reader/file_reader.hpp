#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

/**
 * @defgroup filereader Модуль расширенного файлового ввода
 * @brief Потокобезопасное чтение с кешированием, мониторингом изменений и таймаутами
 */

/**
 * @ingroup filereader
 * @brief Внутреннее состояние файлового потока
 * 
 * Содержит файловый поток и связанные с ним метаданные для управления доступом.
 */
struct FileHandle {
  std::ifstream stream;  ///< Файловый поток ввода
  mutable std::timed_mutex
      mtx;  ///< Мьютекс с поддержкой таймаутов для синхронизации
  fs::file_time_type last_modified;  ///< Время последнего изменения файла
  std::streamsize cached_size =
      -1;  ///< Кешированный размер файла (-1 = не кеширован)
};

/**
 * @ingroup filereader
 * @brief Кешированный блок данных файла
 * 
 * Хранит фрагмент файла с метаданными для управления кешем.
 */
struct CacheBlock {
  std::streampos offset;   ///< Смещение в файле (в байтах)
  std::vector<char> data;  ///< Кешированные данные
  std::chrono::steady_clock::time_point
      timestamp;  ///< Время последнего доступа
};

/**
 * @ingroup filereader
 * @brief Продвинутый читатель файлов с расширенными возможностями
 * 
 * @details Обеспечивает:
 * - Потокобезопасные операции чтения
 * - Кеширование часто используемых данных
 * - Мониторинг изменений файла в реальном времени
 * - Асинхронные операции с таймаутами
 * - Систему подписки на события изменений
 */
class FileReader {
 public:
  /**
   * @brief Конструктор с открытием файла
   * @param path Путь к целевому файлу
   * @param mode Режим открытия файла (по умолчанию: бинарный)
   * @throws std::system_error При ошибке открытия файла
   */
  explicit FileReader(const fs::path& path,
                      std::ios::openmode mode = std::ios::binary);

  ~FileReader();

  FileReader(const FileReader&) = delete;  ///< Копирование запрещено
  FileReader& operator=(const FileReader&) =
      delete;  ///< Копирующее присваивание запрещено

  /**
   * @brief Перемещающий конструктор
   * @param other Объект для перемещения
   */
  FileReader(FileReader&& other) noexcept;

  /**
   * @brief Перемещающий оператор присваивания
   * @param other Объект для перемещения
   * @return Ссылка на текущий объект
   */
  FileReader& operator=(FileReader&& other) noexcept;

  /// @name Основные операции
  /// @{

  /**
   * @brief Чтение всего содержимого файла
   * @return Вектор байтов с содержимым файла
   * @throws std::ios_base::failure При ошибке чтения
   */
  [[nodiscard]] std::vector<char> read() const;

  /**
   * @brief Получение размера файла
   * @param timeout Максимальное время ожидания (по умолчанию: 500 мс)
   * @return Размер файла в байтах
   * @throws TimeoutException При превышении таймаута
   */
  [[nodiscard]] std::streamsize file_size(
      std::chrono::milliseconds timeout = std::chrono::milliseconds(500)) const;

  /// @}

  /// @name Асинхронные операции
  /// @{

  /**
   * @brief Асинхронное чтение файла
   * @return Future с результатом чтения
   */
  std::future<std::vector<char>> async_read() const;

  /// @}

  /// @name Управление кешем
  /// @{

  /**
   * @brief Установка максимального количества кешируемых блоков
   * @param max_blocks Максимальное число блоков в кеше
   */
  void set_cache_size(size_t max_blocks);

  /// @}

  /// @name Система событий
  /// @{

  /**
   * @brief Регистрация обработчика изменений файла
   * @param callback Функция обратного вызова при изменении файла
   * @return Уникальный идентификатор подписки
   */
  [[nodiscard]] size_t subscribe_changes(
      std::function<void(const fs::path&)> callback);

  /**
   * @brief Отмена подписки на изменения
   * @param token Идентификатор подписки
   */
  void unsubscribe_changes(size_t token);

  /// @}

 private:
  /// @name Внутренние методы
  /// @{

  /**
   * @brief Запуск фонового мониторинга файла
   */
  void start_monitoring();

  /**
   * @brief Остановка мониторинга файла
   */
  void stop_monitoring();

  /**
   * @brief Основной цикл мониторинга изменений
   */
  void monitoring_loop();

  /**
   * @brief Очистка устаревших кешированных блоков
   */
  void check_cache_expiration() const;

  /// @}

  fs::path file_path_;                          ///< Абсолютный путь к файлу
  std::unique_ptr<FileHandle> handle_;          ///< Управление файловым потоком
  mutable std::list<CacheBlock> cache_;         ///< LRU-кеш данных файла
  mutable std::shared_mutex cache_mutex_;       ///< Мьютекс для доступа к кешу
  std::atomic<bool> monitoring_active_{false};  ///< Флаг активности мониторинга
  std::thread monitoring_thread_;  ///< Поток для отслеживания изменений
  std::unordered_map<size_t, std::function<void(const fs::path&)>>
      callbacks_;  ///< Обработчики событий
  mutable std::shared_mutex
      callback_mutex_;                  ///< Мьютекс для доступа к обработчикам
  size_t max_cache_blocks_ = 10;        ///< Максимальный размер кеша
  std::atomic<size_t> callback_id_{0};  ///< Счетчик идентификаторов подписок
};
