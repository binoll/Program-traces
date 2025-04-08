/**
 * @file handles.hpp
 * @brief RAII-обертки для ресурсов libregf
 * @details Обеспечивают безопасное управление ресурсами ключей и значений.
*/

#pragma once

#include <libregf.h>

namespace RegistryAnalysis {

/**
 * @class KeyHandle
 * @brief RAII-обёртка для управления ключами реестра
 * @details Автоматически освобождает ресурсы ключа при разрушении объекта.
*/
class KeyHandle {
 public:
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

  /// @brief Деструктор (освобождает ресурсы)
  ~KeyHandle();

  /// @brief Запрещённый конструктор копирования
  KeyHandle(const KeyHandle&) = delete;

  /// @brief Запрещённый оператор присваивания
  KeyHandle& operator=(const KeyHandle&) = delete;

  libregf_key_t* ptr_;  ///< Нативный указатель на ключ из libregf
};

/**
 * @class ValueHandle
 * @brief RAII-обёртка для управления значениями реестра
 * @details Гарантирует безопасное освобождение ресурсов значения.
*/
class ValueHandle {
 public:
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

  /// @brief Деструктор (освобождает ресурсы)
  ~ValueHandle();

  /// @brief Запрещённый конструктор копирования
  ValueHandle(const ValueHandle&) = delete;

  /// @brief Запрещённый оператор присваивания
  ValueHandle& operator=(const ValueHandle&) = delete;

  libregf_value_t* ptr_;  ///< Нативный указатель на значение из libregf
};

}
