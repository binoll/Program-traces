#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "../file_reader/file_reader.hpp"
#include "registry_data_types.hpp"

/**
 * @defgroup registry_analysis Анализ реестра
 * @brief Модуль для работы с файлами реестра Windows
 */

/**
 * @ingroup registry_analysis
 * @brief Анализатор структуры HIVE-файлов
 */
class RegistryAnalyzer {
 public:
  /**
   * @brief Инициализация с указанным файлом реестра
   * @param hive_path Путь к HIVE-файлу
   * @throws HiveValidationError При ошибке валидации
   */
  explicit RegistryAnalyzer(const std::string& hive_path);

  ~RegistryAnalyzer();

  /**
   * @brief Чтение значения из реестра
   * @param key_path Путь к ключу в формате "ROOT\\Subkey"
   * @param value_name Имя целевого значения
   * @param[out] out_type Тип прочитанного значения
   * @return Бинарные данные значения
   * @throws KeyNotFoundError Если ключ не существует
   * @throws ValueNotFoundError Если значение не найдено
   */
  std::vector<uint8_t> read_value(const std::string& key_path,
                                  const std::string& value_name,
                                  ValueType& out_type) const;

  /**
   * @brief Проверка существования ключа
   * @param key_path Путь к целевому ключу
   * @return true если ключ существует, иначе false
   */
  bool key_exists(const std::string& key_path) const noexcept;

  /**
   * @brief Получение версии формата HIVE
   * @return Строка версии в формате major.minor
   */
  [[nodiscard]] std::string hive_version() const;

  /**
   * @brief Статус инициализации анализатора
   * @return true если файл успешно загружен
   */
  [[nodiscard]] bool is_loaded() const noexcept;

 private:
  /// @name Внутренние методы
  /// @{

  void validate_hive_header();
  KeyNode parse_key_header(uint32_t offset) const;
  ValueEntry parse_value_header(uint32_t offset) const;
  KeyNode traverse_key_path(const std::string& key_path) const;
  bool compare_key_name(uint32_t key_offset,
                        const std::string& target_name) const;
  std::vector<uint8_t> read_value_content(const ValueEntry& entry) const;
  std::string decode_utf16_name(const ValueEntry& entry) const;

  /// @}

  /// @name Состояние анализатора
  /// @{

  std::unique_ptr<FileReader> reader_;  ///< Менеджер доступа к файлу
  uint32_t root_key_offset_ = 0;        ///< Смещение корневого ключа
  uint32_t hive_version_ = 0;           ///< Версия формата (major.minor)
  bool is_valid_hive_ = false;          ///< Флаг валидности файла

  /// @}
};
