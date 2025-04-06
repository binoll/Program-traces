/**
 * @file registry_analyzer.hpp
 * @brief Анализатор файлов реестра Windows
 * @details Реализует чтение и анализ файлов реестра с использованием библиотеки libregf
 */

#pragma once

#include <libregf.h>
#include <memory>
#include <string>

namespace RegistryAnalysis {

/**
 * @defgroup Интерфейсы Интерфейсы анализатора
 * @brief Базовые интерфейсы для реализации паттерна "Стратегия"
 * @{
 */

/**
 * @class IRegistryParser
 * @brief Интерфейс обработчика ключей реестра
 * @details Определяет контракт для обработки отдельных ключей реестра
 */
class IRegistryParser {
 public:
  virtual ~IRegistryParser() = default;

  /**
   * @brief Обработка ключа реестра
   * @param[in] key Указатель на структуру ключа реестра
   * @details Вызывается для каждого найденного ключа в процессе анализа
   */
  virtual void ProcessKey(libregf_key_t* key) const = 0;
};

/** @} */

/**
 * @class RegistryAnalyzer
 * @brief Основной класс для анализа файлов реестра
 * @details Обеспечивает чтение файлов реестра и обход структуры ключей
 */
class RegistryAnalyzer {
 public:
  /**
   * @brief Конструктор по умолчанию
   * @throw RegistryInitializationError При ошибке инициализации libregf
   */
  RegistryAnalyzer();

  /**
   * @brief Деструктор
   */
  ~RegistryAnalyzer();

  /**
   * @brief Открыть файл реестра
   * @param[in] file_path Путь к файлу реестра
   * @throw FileOpenError При ошибке открытия файла
   */
  void Open(const std::string& file_path) const;

  /**
   * @brief Запустить анализ реестра
   * @param[in] parser Реализация интерфейса обработки ключей
   * @throw RootKeyError При ошибке получения корневого ключа
   */
  void Analyze(const IRegistryParser& parser) const;

 private:
  /**
   * @class Impl
   * @brief Внутренняя реализация (PIMPL идиома)
   * @details Сокрытие деталей реализации для уменьшения связанности кода
   */
  class Impl {
   public:
    /**
     * @brief Конструктор внутренней реализации
     * @throw RegistryInitializationError При ошибке инициализации
     */
    Impl();

    /**
     * @brief Деструктор внутренней реализации
     */
    ~Impl();

    /**
     * @brief Открытие файла реестра
     * @param[in] file_path Путь к файлу реестра
     * @throw FileOpenError При ошибке открытия файла
     */
    void Open(const std::string& file_path) const;

    /**
     * @brief Выполнение анализа реестра
     * @param[in] parser Обработчик ключей реестра
     * @throw RootKeyError При ошибке получения корневого ключа
     */
    void Analyze(const IRegistryParser& parser) const;

   private:
    libregf_file_t* file_ptr;  ///< Указатель на файл реестра
  };

  std::unique_ptr<Impl>
      impl_;  ///< Уникальный указатель на внутреннюю реализацию
};

}  // namespace RegistryAnalysis
