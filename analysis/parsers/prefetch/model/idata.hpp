/**
 * @file idata.hpp
 * @brief Интерфейс для чтения данных Windows Prefetch-файлов
 */

#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "../metadata/file_metric.hpp"
#include "../metadata/volume_info.hpp"

namespace PrefetchAnalysis {

/**
 * @class IPrefetchData
 * @brief Базовый интерфейс для работы с данными Prefetch-файлов
 * @details Определяет контракт для классов, предоставляющих доступ к данным
 * Prefetch-файлов. Поддерживает все версии Windows от XP до 11.
 * Реализация должна гарантировать потокобезопасность для чтения
 */
class IPrefetchData {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Виртуальный деструктор по умолчанию
   */
  virtual ~IPrefetchData() noexcept = default;
  /// @}

  /// @name Основные методы доступа
  /// @{
  /**
   * @brief Получить имя исполняемого файла
   * @return Строка в формате "NAME.EXE-XXXXXX" (должна быть валидной)
   */
  [[nodiscard]] virtual std::string getExecutableName() const noexcept = 0;

  /**
   * @brief Получить хеш Prefetch-файла
   * @return 32-битный хеш из заголовка файла
   */
  [[nodiscard]] virtual uint32_t getPrefetchHash() const noexcept = 0;

  /**
   * @brief Получить количество запусков приложения
   * @return Число зарегистрированных запусков (≥0)
   */
  [[nodiscard]] virtual uint32_t getRunCount() const noexcept = 0;
  /// @}

  /// @name Методы работы с временными данными
  /// @{
  /**
   * @brief Получить временные метки запусков
   * @return Константная ссылка на вектор временных меток (UNIX time)
   * @note Возвращаемый вектор не должен быть изменяемым
   */
  [[nodiscard]] virtual const std::vector<time_t>& getRunTimes()
      const noexcept = 0;

  /**
   * @brief Получить последнее время выполнения
   * @return Время в формате FILETIME (100-нс интервалы с 01.01.1601)
   */
  [[nodiscard]] virtual time_t getLastRunTime() const noexcept = 0;
  /// @}

  /// @name Методы работы с томами
  /// @{
  /**
   * @brief Получить информацию о томах
   * @return Константная ссылка на вектор объектов VolumeInfo
   */
  [[nodiscard]] virtual const std::vector<VolumeInfo>& getVolumes()
      const noexcept = 0;

  /**
   * @brief Получить основной том выполнения
   * @return Объект VolumeInfo для тома с исполняемым файлом
   * @throw PrefetchDataException Если тома не найдены
   */
  [[nodiscard]] virtual VolumeInfo getMainVolume() const = 0;
  /// @}

  /// @name Методы работы с файловыми метриками
  /// @{
  /**
   * @brief Получить все файловые метрики
   * @return Константная ссылка на вектор объектов FileMetric
   */
  [[nodiscard]] virtual const std::vector<FileMetric>& getMetrics()
      const noexcept = 0;

  /**
   * @brief Получить метрики только для DLL файлов
   * @return Вектор объектов FileMetric для DLL
   */
  [[nodiscard]] virtual std::vector<FileMetric> getDllMetrics() const = 0;
  /// @}

  /**
   * @brief Получить номер версии формата Prefetch
   * @return Числовой идентификатор версии формата (≥10)
   */
  [[nodiscard]] virtual uint8_t getFormatVersion() const noexcept = 0;
  /// @}
};

}
