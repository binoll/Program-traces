#include "value.hpp"

#include <utility>

namespace RegistryAnalysis {

ValueHandle::ValueHandle(libregf_value_t* value) noexcept : ptr_(value) {}

ValueHandle::~ValueHandle() {
  if (ptr_) {
    libregf_value_free(&ptr_, nullptr);
  }
}

ValueHandle::ValueHandle(ValueHandle&& other) noexcept
    : ptr_(std::exchange(other.ptr_, nullptr)) {}

ValueHandle& ValueHandle::operator=(ValueHandle&& other) noexcept {
  if (this != &other) {
    if (ptr_) {
      libregf_value_free(&ptr_, nullptr);
    }
    ptr_ = std::exchange(other.ptr_, nullptr);
  }
  return *this;
}

}
