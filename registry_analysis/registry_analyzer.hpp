/**
 * @file registry_analyzer.hpp
 * @brief Анализатор файлов реестра Windows
 * @details Реализует чтение и анализ файлов реестра с использованием библиотеки libregf.
 *          Поддерживает извлечение ключей, значений и данных различных типов (строки, DWORD, QWORD, бинарные данные).
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
 * @brief Структура для хранения значения реестра
 * @details Содержит имя, данные и тип значения реестра.
 */
struct RegistryValue {
  std::string name;  ///< Имя значения (например, "Version")
  std::string data;  ///< Данные в строковом представлении (например, "1.0.0")
  uint32_t type;     ///< Тип значения (REG_SZ=1, REG_DWORD=4, REG_QWORD=11)
};

/**
 * @class IKeyValueProvider
 * @brief Интерфейс для чтения данных из реестра
 * @details Определяет контракт для классов, предоставляющих доступ к данным реестра.
 */
class IKeyValueProvider {
 public:
  virtual ~IKeyValueProvider() = default;

  /**
   * @brief Получить все значения указанного ключа
   * @param[in] key_path Путь к ключу в формате "ROOT\\Subkey\\..."
   * @return Вектор структур RegistryValue с данными ключа
   * @throw SubkeyNotFoundError Если ключ не существует
   * @throw std::runtime_error При ошибках чтения
   */
  [[nodiscard]] virtual std::vector<RegistryValue> GetAllKeyValues(
      const std::string& key_path) const = 0;

  /**
   * @brief Получить конкретное значение по имени
   * @param[in] key_path Путь к ключу
   * @param[in] value_name Имя значения (например, "ProductName")
   * @return Заполненная структура RegistryValue
   * @throw ValueNotFoundError Если значение не найдено
   * @throw SubkeyNotFoundError Если ключ не существует
   */
  [[nodiscard]] virtual RegistryValue GetValueByName(
      const std::string& key_path, const std::string& value_name) const = 0;
};

/**
 * @class KeyHandle
 * @brief RAII-обёртка для управления ключами реестра
 * @details Автоматически освобождает ресурсы при выходе из области видимости.
 */
struct KeyHandle {
  libregf_key_t* ptr;  ///< Указатель на ключ реестра

  /**
   * @brief Конструктор из сырого указателя
   * @param[in] key Указатель на ключ (может быть nullptr)
   */
  explicit KeyHandle(libregf_key_t* key = nullptr) noexcept : ptr(key) {}

  ~KeyHandle() = default;
};

/**
 * @class ValueHandle
 * @brief RAII-обёртка для управления значениями реестра
 * @details Гарантирует корректное освобождение ресурсов.
 */
struct ValueHandle {
  libregf_value_t* ptr;  ///< Указатель на значение реестра

  /**
   * @brief Конструктор из сырого указателя
   * @param[in] value Указатель на значение (может быть nullptr)
   */
  explicit ValueHandle(libregf_value_t* value = nullptr) noexcept
      : ptr(value) {}

  ~ValueHandle() {
    if (ptr)
      libregf_value_free(&ptr, nullptr);
  }
};

/**
 * @class RegistryAnalyzer
 * @brief Основной класс для работы с файлами реестра
 * @implements IKeyValueProvider
 * @details Использует PIMPL идиому для сокрытия деталей реализации.
 */
class RegistryAnalyzer : public IKeyValueProvider {
 public:
  RegistryAnalyzer();
  ~RegistryAnalyzer() override;

  /**
   * @brief Открыть файл реестра
   * @param[in] file_path Путь к файлу (например, "C:\\Windows\\system32\\config\\SOFTWARE")
   * @throw FileOpenError При ошибках доступа к файлу
   */
  void Open(const std::string& file_path) const;

  /**
   * @brief Получить все значения ключа
   * @details Рекурсивно обходит подразделы и возвращает все значения.
   * @param[in] key_path Путь к ключу реестра
   * @throw SubkeyNotFoundError Если путь содержит несуществующие подразделы
   */
  [[nodiscard]] std::vector<RegistryValue> GetAllKeyValues(
      const std::string& key_path) const override;

  /**
   * @brief Получить значение по имени
   * @param[in] key_path Путь к целевому ключу
   * @param[in] value_name Имя значения (регистрозависимое)
   * @throw ValueNotFoundError Если значение отсутствует в ключе
   */
  [[nodiscard]] RegistryValue GetValueByName(
      const std::string& key_path,
      const std::string& value_name) const override;

 private:
  /**
   * @class Impl
   * @brief Внутренняя реализация работы с libregf
   * @details Содержит методы для низкоуровневого взаимодействия с библиотекой.
   */
  class Impl {
   public:
    Impl();
    ~Impl();

    /**
     * @brief Открыть файл реестра (внутренняя реализация)
     * @throw FileOpenError При ошибках чтения файла
     */
    void Open(const std::string& file_path) const;

    /// @copydoc RegistryAnalyzer::GetAllKeyValues
    [[nodiscard]] std::vector<RegistryValue> GetAllKeyValues(
        const std::string& key_path) const;

    /// @copydoc RegistryAnalyzer::GetValueByName
    [[nodiscard]] RegistryValue GetValueByName(
        const std::string& key_path, const std::string& value_name) const;

   private:
    /**
     * @brief Разрешить путь к ключу
     * @param[in] path Путь вида "ROOT\\Subkey1\\Subkey2"
     * @throw RootKeyError При ошибке доступа к корневому ключу
     */
    [[nodiscard]] KeyHandle ResolveKeyPath(const std::string& path) const;

    /**
     * @brief Извлечь данные значения реестра
     * @param[in] value Указатель на объект значения
     * @return Заполненная структура RegistryValue
     */
    static RegistryValue ExtractValueData(libregf_value_t* value);

    /// @brief Извлечь строковое значение
    static void ExtractString(libregf_value_t* value, std::string& output);

    /// @brief Извлечь 32-битное целое (DWORD)
    static void ExtractDword(libregf_value_t* value, std::string& output);

    /// @brief Извлечь 64-битное целое (QWORD)
    static void ExtractQword(libregf_value_t* value, std::string& output);

    /// @brief Извлечь бинарные данные в hex-строку
    static void ExtractBinary(libregf_value_t* value, std::string& output);

    libregf_file_t* file_ = nullptr;  ///< Указатель на открытый файл реестра
  };

  std::unique_ptr<Impl> impl_;  ///< Указатель на внутреннюю реализацию
};

}  // namespace RegistryAnalysis
