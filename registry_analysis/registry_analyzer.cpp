/**
 * @file registry_analyzer.cpp
 * @brief Реализация методов анализатора реестра
 */

#include "registry_analyzer.hpp"
#include <libregf.h>

#include <iomanip>
#include <sstream>
#include <vector>
#include "registry_analyzer_exceptions.hpp"

namespace RegistryAnalysis {

// Реализация методов Impl
RegistryAnalyzer::Impl::Impl() {
  if (libregf_file_initialize(&file_, nullptr) != 1) {
    throw RegistryInitializationError();
  }
}

RegistryAnalyzer::Impl::~Impl() {
  if (file_) {
    libregf_file_free(&file_, nullptr);
  }
}

void RegistryAnalyzer::Impl::Open(const std::string& file_path) const {
  if (libregf_file_open(file_, file_path.c_str(), LIBREGF_OPEN_READ, nullptr) !=
      1) {
    throw FileOpenError(file_path);
  }
}

std::vector<RegistryValue> RegistryAnalyzer::Impl::GetAllKeyValues(
    const std::string& key_path) const {
  const KeyHandle key = ResolveKeyPath(key_path);
  std::vector<RegistryValue> values;

  int value_count = 0;
  if (libregf_key_get_number_of_values(key.ptr, &value_count, nullptr) != 1) {
    return values;
  }

  for (int i = 0; i < value_count; ++i) {
    ValueHandle value;
    if (libregf_key_get_value_by_index(key.ptr, i, &value.ptr, nullptr) == 1) {
      values.push_back(ExtractValueData(value.ptr));
    }
  }

  return values;
}

RegistryValue RegistryAnalyzer::Impl::GetValueByName(
    const std::string& key_path, const std::string& value_name) const {
  const KeyHandle key = ResolveKeyPath(key_path);
  ValueHandle value;

  if (libregf_key_get_value_by_utf8_name(
          key.ptr, reinterpret_cast<const uint8_t*>(value_name.c_str()), 0,
          &value.ptr, nullptr) != 1) {
    throw ValueNotFoundError(value_name);
  }

  return ExtractValueData(value.ptr);
}

KeyHandle RegistryAnalyzer::Impl::ResolveKeyPath(
    const std::string& path) const {
  KeyHandle root_key;
  if (libregf_file_get_root_key(file_, &root_key.ptr, nullptr) != 1) {
    throw RootKeyError();
  }

  if (path.empty()) {
    return root_key;
  }

  size_t start = 0;
  size_t end;
  KeyHandle current_key = root_key;

  while ((end = path.find('/', start)) != std::string::npos) {
    const std::string token = path.substr(start, end - start);
    KeyHandle next_key;

    if (libregf_key_get_sub_key_by_utf8_name(
            current_key.ptr, reinterpret_cast<const uint8_t*>(token.c_str()),
            token.size(), &next_key.ptr, nullptr) != 1) {
      throw SubkeyNotFoundError(token);
    }

    current_key = next_key;
    start = end + 1;
  }

  const std::string last_token = path.substr(start);
  KeyHandle final_key;
  if (libregf_key_get_sub_key_by_utf8_name(
          current_key.ptr, reinterpret_cast<const uint8_t*>(last_token.c_str()),
          last_token.size(), &final_key.ptr, nullptr) != 1) {
    throw SubkeyNotFoundError(last_token);
  }

  return final_key;
}

RegistryValue RegistryAnalyzer::Impl::ExtractValueData(libregf_value_t* value) {
  RegistryValue result;

  // Получение имени значения
  size_t name_size = 0;
  if (libregf_value_get_utf8_name_size(value, &name_size, nullptr) == 1 &&
      name_size > 0) {
    std::vector<uint8_t> buffer(name_size);
    if (libregf_value_get_utf8_name(value, buffer.data(), name_size, nullptr) ==
        1) {
      result.name.assign(reinterpret_cast<char*>(buffer.data()), name_size - 1);
    }
  }

  // Получение типа значения
  libregf_value_get_value_type(value, &result.type, nullptr);

  // Обработка данных в зависимости от типа
  switch (result.type) {
    case 1:  // REG_SZ
    case 2:  // REG_EXPAND_SZ
      ExtractString(value, result.data);
      break;
    case 4:  // REG_DWORD
      ExtractDword(value, result.data);
      break;
    case 11:  // REG_QWORD
      ExtractQword(value, result.data);
      break;
    default:
      ExtractBinary(value, result.data);
      break;
  }

  return result;
}

void RegistryAnalyzer::Impl::ExtractString(libregf_value_t* value,
                                           std::string& output) {
  // Шаг 1: Получение размера данных через отдельную функцию
  size_t data_size = 0;
  if (libregf_value_get_value_data_size(value, &data_size, nullptr) != 1) {
    return;
  }

  // Шаг 2: Выделение буфера с учетом размера
  std::vector<uint8_t> buffer(data_size);

  // Шаг 3: Получение данных
  size_t actual_size = data_size;
  if (libregf_value_get_value_utf8_string(value, buffer.data(), buffer.size(),
                                          nullptr) == 1) {
    // Копирование данных без учета null-терминатора (если он есть)
    output.assign(reinterpret_cast<char*>(buffer.data()), actual_size);
  }
}

void RegistryAnalyzer::Impl::ExtractDword(libregf_value_t* value,
                                          std::string& output) {
  uint32_t data;
  if (libregf_value_get_value_32bit(value, &data, nullptr) == 1) {
    output = std::to_string(data);
  }
}

void RegistryAnalyzer::Impl::ExtractQword(libregf_value_t* value,
                                          std::string& output) {
  uint64_t data;
  if (libregf_value_get_value_64bit(value, &data, nullptr) == 1) {
    output = std::to_string(data);
  }
}

void RegistryAnalyzer::Impl::ExtractBinary(libregf_value_t* value,
                                           std::string& output) {
  std::vector<uint8_t> buffer(256);
  bool data_read = false;

  while (!data_read) {
    int result = libregf_value_get_value_data(value, buffer.data(),
                                              buffer.size(), nullptr);

    if (result == 1) {
      data_read = true;
    } else if (result == -1 && buffer.size() < 1024 * 1024) {
      buffer.resize(buffer.size() * 2);
    } else {
      throw std::runtime_error("Ошибка чтения бинарных данных");
    }
  }

  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (uint8_t byte : buffer) {
    oss << std::setw(2) << static_cast<int>(byte);
  }
  output = oss.str();
}

// Реализация публичных методов
RegistryAnalyzer::RegistryAnalyzer() : impl_(std::make_unique<Impl>()) {}

RegistryAnalyzer::~RegistryAnalyzer() = default;

void RegistryAnalyzer::Open(const std::string& file_path) const {
  impl_->Open(file_path);
}

std::vector<RegistryValue> RegistryAnalyzer::GetAllKeyValues(
    const std::string& key_path) const {
  return impl_->GetAllKeyValues(key_path);
}

RegistryValue RegistryAnalyzer::GetValueByName(
    const std::string& key_path, const std::string& value_name) const {
  return impl_->GetValueByName(key_path, value_name);
}

}  // namespace RegistryAnalysis
