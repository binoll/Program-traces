/**
 * @file key.hpp
 * @brief RAII-обертка для управления ключами реестра Windows
 */

#pragma once

#include <libregf.h>

namespace RegistryAnalysis {

/**
 * @class KeyHandle
 * @brief Интеллектуальный указатель для работы с ключами реестра
 */
class KeyHandle {
 public:
  /// @name Основные методы класса
  /// @{
  /**
   * @brief Основной конструктор
   * @param[in] key Указатель на ключ реестра (может быть nullptr)
   * @note Принимает владение переданным ресурсом. Не требует проверки на
   * nullptr
   */
  explicit KeyHandle(libregf_key_t* key = nullptr) noexcept;

  /**
   * @brief Деструктор
   * @note Гарантирует освобождение ресурсов через libregf_key_free
   */
  ~KeyHandle();

  /**
   * @brief Конструктор перемещения
   * @param[in, out] other Источник данных (после перемещения other.ptr_ =
   * nullptr)
   * @post После перемещения other.ptr_ == nullptr
   */
  KeyHandle(KeyHandle&& other) noexcept;

  /**
   * @brief Оператор перемещающего присваивания
   * @param[in, out] other Источник данных (после операции other.ptr_ = nullptr)
   * @return *this
   * @note Выполняет безопасный обмен ресурсами с предварительным освобождением
   */
  KeyHandle& operator=(KeyHandle&& other) noexcept;
  /// @}

  /// @name Запрещённые операции
  /// @{
  /**
   * @brief Запрещённый конструктор копирования
   * @param[in] other Объект для копирования
   * @throw NonCopyableError При попытке копирования
   */
  KeyHandle(const KeyHandle& other) = delete;

  /**
   * @brief Запрещённый оператор присваивания
   * @param[in] other Источник для копирования
   * @return KeyHandle& Недоступен
   * @throw NonCopyableError Всегда при вызове
   */
  KeyHandle& operator=(const KeyHandle& other) = delete;
  /// @}

  libregf_key_t* ptr_;  ///< Управляемый указатель на ключ реестра
};

}
