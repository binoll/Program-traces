#include "key.hpp"

#include <libregf.h>

namespace RegistryAnalysis {

KeyHandle::KeyHandle(libregf_key_t* key) noexcept : ptr_(key) {}

KeyHandle::KeyHandle(KeyHandle&& other) noexcept : ptr_(other.ptr_) {
  other.ptr_ = nullptr;
}

KeyHandle& KeyHandle::operator=(KeyHandle&& other) noexcept {
  if (this != &other) {
    if (ptr_) {
      libregf_key_free(&ptr_, nullptr);
    }
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
  }
  return *this;
}

KeyHandle::~KeyHandle() {
  if (ptr_) {
    libregf_key_free(&ptr_, nullptr);
  }
}

}
