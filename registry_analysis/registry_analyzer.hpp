/**
 * @file registry_analyzer.hpp
 * @brief Анализатор файлов реестра Windows
 * @details Реализует чтение и анализ файлов реестра с использованием библиотеки libregf.
*/

#pragma once

#include <libregf.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace RegistryAnalysis {

/**
 * @struct RegistryValue
 * @brief Контейнер данных значения реестра
 * @details Содержит имя, данные и тип значения в унифицированном формате.
*/
struct RegistryValue {
  std::string name;  ///< Имя значения в кодировке UTF-8
  std::string data;  ///< Данные в строковом представлении
  uint32_t type;     ///< Тип значения (REG_SZ=1, REG_DWORD=4, REG_QWORD=11)
};

/**
 * @interface IKeyValueProvider
 * @brief Интерфейс для доступа к данным реестра
 * @details Определяет контракт для чтения ключей и значений реестра.
*/
class IKeyValueProvider {
 public:
  virtual ~IKeyValueProvider() = default;

  /**
   * @brief Получить все значения ключа
   * @param[in] key_path Путь к ключу в формате "ROOT\\Подраздел\\..."
   * @return Вектор значений типа RegistryValue
   * @throw SubkeyNotFoundError Если подраздел не найден
   * @throw RootKeyError Если корневой ключ недоступен
  */
  [[nodiscard]] virtual std::vector<RegistryValue> GetAllKeyValues(
      const std::string& key_path) const = 0;

  /**
   * @brief Получить значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя значения (регистрозависимое)
   * @return Структура RegistryValue с данными
   * @throw ValueNotFoundError Если значение отсутствует
   * @throw SubkeyNotFoundError Если ключ не существует
  */
  [[nodiscard]] virtual RegistryValue GetValueByName(
      const std::string& key_path, const std::string& value_name) const = 0;
};

/**
 * @class KeyHandle
 * @brief RAII-обёртка для управления ключами реестра
 * @details Автоматически освобождает ресурсы ключа при разрушении объекта.
*/
struct KeyHandle {
  libregf_key_t* ptr;  ///< Нативный указатель на ключ из libregf

  /**
   * @brief Конструктор из сырого указателя
   * @param[in] key Указатель на ключ (может быть nullptr)
  */
  explicit KeyHandle(libregf_key_t* key = nullptr) noexcept;

  /**
   * @brief Конструктор перемещения
   * @param[in] other Временный объект KeyHandle
  */
  KeyHandle(KeyHandle&& other) noexcept;

  /**
   * @brief Оператор перемещающего присваивания
   * @param[in] other Временный объект KeyHandle
  */
  KeyHandle& operator=(KeyHandle&& other) noexcept;

  /**
   * @brief Деструктор (освобождает ресурсы)
  */
  ~KeyHandle();

  KeyHandle(const KeyHandle&) = delete;
  KeyHandle& operator=(const KeyHandle&) = delete;
};

/**
 * @class ValueHandle
 * @brief RAII-обёртка для управления значениями реестра
 * @details Гарантирует безопасное освобождение ресурсов значения.
*/
struct ValueHandle {
  libregf_value_t* ptr;  ///< Нативный указатель на значение из libregf

  /**
   * @brief Конструктор из сырого указателя
   * @param[in] value Указатель на значение (может быть nullptr)
  */
  explicit ValueHandle(libregf_value_t* value = nullptr) noexcept;

  /**
   * @brief Конструктор перемещения
   * @param[in] other Временный объект ValueHandle
  */
  ValueHandle(ValueHandle&& other) noexcept;

  /**
   * @brief Оператор перемещающего присваивания
   * @param[in] other Временный объект ValueHandle
  */
  ValueHandle& operator=(ValueHandle&& other) noexcept;

  /**
   * @brief Деструктор (освобождает ресурсы)
  */
  ~ValueHandle();

  ValueHandle(const ValueHandle&) = delete;
  ValueHandle& operator=(const ValueHandle&) = delete;
};

/**
 * @class RegistryAnalyzer
 * @brief Основной класс для работы с файлами реестра
 * @implements IKeyValueProvider
 * @details Использует PIMPL для изоляции зависимостей от библиотеки libregf.
*/
class RegistryAnalyzer : public IKeyValueProvider {
 public:
  RegistryAnalyzer();
  ~RegistryAnalyzer() override;

  /**
   * @brief Открыть файл реестра для анализа
   * @param[in] file_path Путь к файлу (например, "C:\\Windows\\system32\\config\\SOFTWARE")
   * @throw FileOpenError При ошибке открытия файла
  */
  void Open(const std::string& file_path) const;

  /**
   * @brief Получить все значения ключа
   * @details Рекурсивно обходит подразделы и возвращает все значения.
   * @param[in] key_path Путь к ключу реестра
   * @throw SubkeyNotFoundError Если подраздел не существует
  */
  [[nodiscard]] std::vector<RegistryValue> GetAllKeyValues(
      const std::string& key_path) const override;

  /**
   * @brief Получить значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя значения (регистрозависимое)
   * @throw ValueNotFoundError Если значение не найдено
  */
  [[nodiscard]] RegistryValue GetValueByName(
      const std::string& key_path,
      const std::string& value_name) const override;

 private:
  /// @brief Внутренняя реализация (PIMPL)
  class Impl;
  std::unique_ptr<Impl> impl_;
};

/**
 * @class RegistryAnalyzer::Impl
 * @brief Внутренняя реализация анализатора реестра
 * @details Содержит низкоуровневую логику работы с библиотекой libregf.
*/
class RegistryAnalyzer::Impl {
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
   * @brief Открыть файл реестра
   * @param[in] file_path Путь к файлу реестра
   * @throw FileOpenError При ошибке доступа
  */
  void Open(const std::string& file_path) const;

  /**
   * @brief Получить все значения ключа
   * @param[in] key_path Путь к ключу в формате "ROOT\\Подраздел\\..."
   * @throw SubkeyNotFoundError Если путь содержит несуществующие подразделы
  */
  [[nodiscard]] std::vector<RegistryValue> GetAllKeyValues(
      const std::string& key_path) const;

  /**
   * @brief Получить значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя значения
   * @throw ValueNotFoundError Если значение отсутствует
  */
  [[nodiscard]] RegistryValue GetValueByName(
      const std::string& key_path, const std::string& value_name) const;

 private:
  /**
   * @brief Разрешить путь к ключу
   * @param[in] path Путь вида "ROOT\\Подраздел1\\Подраздел2"
   * @throw SubkeyNotFoundError Если подраздел отсутствует
  */
  [[nodiscard]] KeyHandle ResolveKeyPath(const std::string& path) const;

  /**
   * @brief Извлечь данные значения реестра
   * @param[in] value Указатель на объект значения
  */
  static RegistryValue ExtractValueData(libregf_value_t* value);

  /// @brief Извлечь строковое значение (REG_SZ, REG_EXPAND_SZ)
  static void ExtractString(libregf_value_t* value, std::string& output);

  /// @brief Извлечь 32-битное целое (REG_DWORD)
  static void ExtractDword(libregf_value_t* value, std::string& output);

  /// @brief Извлечь 64-битное целое (REG_QWORD)
  static void ExtractQword(libregf_value_t* value, std::string& output);

  /// @brief Извлечь бинарные данные (REG_BINARY)
  static void ExtractBinary(libregf_value_t* value, std::string& output);

  libregf_file_t* file_ = nullptr;  ///< Указатель на открытый файл реестра
};

}
