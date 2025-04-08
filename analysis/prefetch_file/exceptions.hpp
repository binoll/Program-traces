/**
 * @file exceptions.hpp
 * @brief Пользовательские исключения для анализа Prefetch-файлов
 * @details Содержит иерархию классов исключений для обработки ошибок:
 *          - Работа с томами
 *          - Файловые метрики
 *          - Обработка Prefetch-данных
 *          - Парсинг файлов
 */

#pragma once

#include <stdexcept>
#include <string>

namespace PrefetchAnalysis {

/*================================ Томы ======================================*/

/**
 * @brief Базовое исключение для ошибок работы с логическими томами
 * @details Все исключения связанные с томами наследуются от этого класса
 */
class VolumeException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор базового исключения тома
   * @param[in] msg Сообщение об ошибке в кодировке UTF-8
   */
  explicit VolumeException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Исключение некорректного пути устройства
 * @details Возникает при операциях с невалидными NT-путями формата:
 *          "\Device\HarddiskVolumeX"
 */
class InvalidDevicePathException : public VolumeException {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] path Некорректный путь, вызвавший ошибку
   */
  explicit InvalidDevicePathException(const std::string& path)
      : VolumeException("Некорректный путь к устройству: " + path) {}
};

/*============================ Файловые метрики ==============================*/

/**
 * @brief Базовое исключение для ошибок файловых метрик
 * @details Объединяет ошибки связанные с обработкой метрик файлов
 */
class FileMetricException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор базового исключения метрик
   * @param[in] msg Описание ошибки в кодировке UTF-8
   */
  explicit FileMetricException(const std::string& msg)
      : std::runtime_error(msg) {}
};

/**
 * @brief Исключение невалидных параметров файла
 * @details Генерируется при:
 *          - Нарушении формата имени файла
 *          - Некорректной MFT-ссылке
 */
class InvalidFileMetricException : public FileMetricException {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] context Контекст ошибки для диагностики
   */
  explicit InvalidFileMetricException(const std::string& context)
      : FileMetricException("Некорректные параметры файла: " + context) {}
};

/*=========================== Обработка Prefetch =============================*/

/**
 * @brief Базовое исключение для ошибок обработки Prefetch-данных
 * @details Родитель для ошибок возникающих при работе с содержимым Prefetch-файлов
 */
class PrefetchDataException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор базового исключения
   * @param[in] msg Описание ошибки в кодировке UTF-8
   */
  explicit PrefetchDataException(const std::string& msg)
      : std::runtime_error(msg) {}
};

/**
 * @brief Исключение некорректного имени исполняемого файла
 * @details Срабатывает при:
 *          - Пустом имени
 *          - Наличии запрещенных символов
 *          - Превышении максимальной длины
 */
class InvalidExecutableNameException : public PrefetchDataException {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] name Некорректное имя, вызвавшее ошибку
   */
  explicit InvalidExecutableNameException(const std::string& name)
      : PrefetchDataException("Некорректное имя исполняемого файла: " + name) {}
};

/*================================ Парсинг ===================================*/

/**
 * @brief Базовое исключение для ошибок парсинга
 * @details Объединяет ошибки на различных этапах разбора файлов
 */
class ParsingException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор базового исключения парсинга
   * @param[in] msg Детальное описание ошибки
   */
  explicit ParsingException(const std::string& msg)
      : std::runtime_error("Ошибка парсинга: " + msg) {}
};

/**
 * @brief Исключение ошибки открытия файла
 * @details Возникает при:
 *          - Отсутствии файла по указанному пути
 *          - Отсутствии прав доступа
 *          - Повреждении файловой системы
 */
class FileOpenException : public ParsingException {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] path Путь к файлу вызвавшему ошибку
   */
  explicit FileOpenException(const std::string& path)
      : ParsingException("Не удалось открыть файл: " + path) {}
};

/**
 * @brief Исключение некорректного формата данных
 * @details Генерируется при обнаружении структурных нарушений в файле
 */
class InvalidFormatException : public ParsingException {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] details Технические детали ошибки
   */
  explicit InvalidFormatException(const std::string& details)
      : ParsingException("Некорректный формат данных: " + details) {}
};

/**
 * @brief Исключение ошибки чтения данных
 * @details Возникает при сбоях операций ввода-вывода
 */
class DataReadException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] context Контекст операции чтения
   */
  explicit DataReadException(const std::string& context)
      : std::runtime_error("Ошибка чтения данных: " + context) {}
};

/**
 * @brief Исключение некорректной временной метки
 * @details Срабатывает при:
 *          - Времени до эпохи UNIX (1 января 1970)
 *          - Слишком больших значениях (> 30828 года)
 */
class InvalidTimestampException : public std::runtime_error {
 public:
  /**
   * @brief Конструктор исключения
   * @param[in] details Технические детали ошибки
   */
  explicit InvalidTimestampException(const std::string& details)
      : std::runtime_error("Некорректная временная метка: " + details) {}
};

}
