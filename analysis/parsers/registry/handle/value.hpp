/**
 * @file value.hpp
 * @brief RAII-обёртка для управления значениями реестра Windows
 */

#pragma once

#include <libregf.h>

namespace RegistryAnalysis {

/**
 * @class ValueHandle
 * @brief Интеллектуальный указатель для работы со значениями реестра
 */
class ValueHandle {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Основной конструктор
   * @param[in] value Указатель на значение реестра (может быть nullptr)
   * @note Принимает владение ресурсом. Не требует проверки на nullptr
   */
  explicit ValueHandle(libregf_value_t* value = nullptr) noexcept;

  /**
   * @brief Деструктор
   * @note Гарантирует освобождение ресурсов через libregf_value_free
   */
  ~ValueHandle();

  /**
   * @brief Конструктор перемещения
   * @param[in, out] other Источник данных (после перемещения other.ptr_ =
   * nullptr)
   * @post После перемещения other.ptr_ == nullptr
   */
  ValueHandle(ValueHandle&& other) noexcept;

  /**
   * @brief Оператор перемещающего присваивания
   * @param[in, out] other Источник данных (после операции other.ptr_ = nullptr)
   * @return *this
   * @note Выполняет безопасный обмен ресурсами
   */
  ValueHandle& operator=(ValueHandle&& other) noexcept;
  /// @}

  /// @name Запрещённые операции
  /// @{
  /**
   * @brief Запрещённый конструктор копирования
   * @param[in] other Объект для копирования
   * @throw NonCopyableError При попытке копирования
   */
  ValueHandle(const ValueHandle& other) = delete;

  /**
   * @brief Запрещённый оператор присваивания
   * @param[in] other Источник для копирования
   * @return ValueHandle& Недоступен
   * @throw NonCopyableError Всегда при вызове
   */
  ValueHandle& operator=(const ValueHandle& other) = delete;
  /// @}

  libregf_value_t* ptr_;  ///< Управляемый указатель на значение реестра
};

}
