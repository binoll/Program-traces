/**
 * @file registry_analyzer.cpp
 * @brief Реализация методов анализатора реестра
 */

#include "registry_analyzer.hpp"
#include <libregf.h>
#include <stack>
#include "registry_analyzer_exceptions.hpp"

namespace RegistryAnalysis {

/*
 * Реализация методов внутреннего класса
 */
RegistryAnalyzer::Impl::Impl() : file_ptr(nullptr) {
  if (libregf_file_initialize(&file_ptr, nullptr) != 1) {
    throw RegistryInitializationError();
  }
}

RegistryAnalyzer::Impl::~Impl() {
  if (file_ptr) {
    libregf_file_close(file_ptr, nullptr);
    libregf_file_free(&file_ptr, nullptr);
  }
}

void RegistryAnalyzer::Impl::Open(const std::string& file_path) const {
  if (libregf_file_open(file_ptr, file_path.c_str(), LIBREGF_OPEN_READ,
                        nullptr) != 1) {
    throw FileOpenError(file_path);
  }
}

void RegistryAnalyzer::Impl::Analyze(const IRegistryParser& parser) const {
  libregf_key_t* root_key = nullptr;
  if (libregf_file_get_root_key(file_ptr, &root_key, nullptr) != 1) {
    throw RootKeyError();
  }

  std::stack<libregf_key_t*> key_stack;
  key_stack.push(root_key);

  while (!key_stack.empty()) {
    libregf_key_t* current_key = key_stack.top();
    key_stack.pop();

    // Обработка текущего ключа
    parser.ProcessKey(current_key);

    // Получение количества подразделов
    int subkey_count = 0;
    if (libregf_key_get_number_of_sub_keys(current_key, &subkey_count,
                                           nullptr) != 1) {
      continue;
    }

    // Добавление подразделов в стек
    for (int i = 0; i < subkey_count; ++i) {
      libregf_key_t* sub_key = nullptr;
      if (libregf_key_get_sub_key_by_index(current_key, i, &sub_key, nullptr) ==
          1) {
        key_stack.push(sub_key);
      }
    }

    // Освобождение памяти для не корневых ключей
    if (current_key != root_key) {
      libregf_key_free(&current_key, nullptr);
    }
  }

  libregf_key_free(&root_key, nullptr);
}

/*
 * Реализация методов внешнего класса
 */
RegistryAnalyzer::RegistryAnalyzer() : impl_(std::make_unique<Impl>()) {}

RegistryAnalyzer::~RegistryAnalyzer() = default;

void RegistryAnalyzer::Open(const std::string& file_path) const {
  impl_->Open(file_path);
}

void RegistryAnalyzer::Analyze(const IRegistryParser& parser) const {
  impl_->Analyze(parser);
}

}  // namespace RegistryAnalysis
