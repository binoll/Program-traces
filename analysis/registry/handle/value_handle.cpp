/**
 * @file value_handle.cpp
 * @brief Реализация RAII-обёртки для работы со значениями реестра.
*/

#include "value_handle.hpp"
#include <libregf.h>

namespace RegistryAnalysis {

ValueHandle::ValueHandle(libregf_value_t* value) noexcept : ptr_(value) {}

ValueHandle::ValueHandle(ValueHandle&& other) noexcept : ptr_(other.ptr_) {
  other.ptr_ = nullptr;
}

ValueHandle& ValueHandle::operator=(ValueHandle&& other) noexcept {
  if (this != &other) {
    if (ptr_) {
      libregf_value_free(&ptr_, nullptr);
    }
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
  }
  return *this;
}

ValueHandle::~ValueHandle() {
  if (ptr_) {
    libregf_value_free(&ptr_, nullptr);
  }
}

}
