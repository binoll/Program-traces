/**
 * @file registry_parser.cpp
 * @brief Полная реализация методов парсера реестра Windows
 * @details Содержит внутреннюю реализацию работы с libregf через PIMPL
*/

#include "registry_parser.hpp"
#include <libregf.h>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>
#include "exceptions.hpp"
#include "handles.hpp"
#include "models.hpp"

namespace RegistryParser {

RegistryParser::Impl::Impl() {
  if (libregf_file_initialize(&file_, nullptr) != 1) {
    throw RegistryInitializationError();
  }
}

RegistryParser::Impl::~Impl() {
  if (file_) {
    libregf_file_free(&file_, nullptr);
  }
}

void RegistryParser::Impl::open(const std::string& file_path) const {
  if (libregf_file_open(file_, file_path.c_str(), LIBREGF_OPEN_READ, nullptr) !=
      1) {
    throw FileOpenError(file_path);
  }
}

std::vector<RegistryValue> RegistryParser::Impl::getAllKeyValues(
    const std::string& key_path) const {
  const KeyHandle key = resolveKeyPath(key_path);
  std::vector<RegistryValue> values;

  int value_count = 0;
  if (libregf_key_get_number_of_values(key.ptr_, &value_count, nullptr) != 1) {
    return values;
  }

  for (int i = 0; i < value_count; ++i) {
    ValueHandle value;
    if (libregf_key_get_value_by_index(key.ptr_, i, &value.ptr_, nullptr) ==
        1) {
      values.push_back(extractValueData(value.ptr_));
    }
  }

  return values;
}

RegistryValue RegistryParser::Impl::getValueByName(
    const std::string& key_path, const std::string& value_name) const {
  const KeyHandle key = resolveKeyPath(key_path);
  ValueHandle value;

  if (libregf_key_get_value_by_utf8_name(
          key.ptr_, reinterpret_cast<const uint8_t*>(value_name.c_str()),
          value_name.size(), &value.ptr_, nullptr) != 1) {
    throw ValueNotFoundError(value_name);
  }

  return extractValueData(value.ptr_);
}

KeyHandle RegistryParser::Impl::resolveKeyPath(const std::string& path) const {
  KeyHandle root_key;
  if (libregf_file_get_root_key(file_, &root_key.ptr_, nullptr) != 1) {
    throw RootKeyError();
  }

  if (path.empty()) {
    return std::move(root_key);
  }

  size_t start = 0;
  KeyHandle current_key = std::move(root_key);

  while (true) {
    size_t end = path.find('/', start);
    if (end == std::string::npos) {
      break;
    }

    const std::string token = path.substr(start, end - start);
    KeyHandle next_key;
    if (libregf_key_get_sub_key_by_utf8_name(
            current_key.ptr_, reinterpret_cast<const uint8_t*>(token.c_str()),
            token.size(), &next_key.ptr_, nullptr) != 1) {
      throw SubkeyNotFoundError(token);
    }

    current_key = std::move(next_key);
    start = end + 1;
  }

  const std::string last_token = path.substr(start);
  KeyHandle final_key;
  if (libregf_key_get_sub_key_by_utf8_name(
          current_key.ptr_,
          reinterpret_cast<const uint8_t*>(last_token.c_str()),
          last_token.size(), &final_key.ptr_, nullptr) != 1) {
    throw SubkeyNotFoundError(last_token);
  }

  return final_key;
}

RegistryValue RegistryParser::Impl::extractValueData(libregf_value_t* value) {
  RegistryValue result;

  // Извлечение имени значения
  size_t name_size = 0;
  if (libregf_value_get_utf8_name_size(value, &name_size, nullptr) == 1 &&
      name_size > 0) {
    std::vector<uint8_t> buffer(name_size);
    if (libregf_value_get_utf8_name(value, buffer.data(), name_size, nullptr) ==
        1) {
      result.name_.assign(reinterpret_cast<char*>(buffer.data()),
                          name_size - 1);
    }
  }

  // Определение типа и данных значения
  libregf_value_get_value_type(value, &result.type_, nullptr);

  switch (result.type_) {
    case 1:  // REG_SZ
    case 2:  // REG_EXPAND_SZ
      extractString(value, result.data_);
      break;
    case 4:  // REG_DWORD
      extractDword(value, result.data_);
      break;
    case 11:  // REG_QWORD
      extractQword(value, result.data_);
      break;
    default:  // Остальные типы как бинарные
      extractBinary(value, result.data_);
      break;
  }

  return result;
}

void RegistryParser::Impl::extractString(libregf_value_t* value,
                                         std::string& output) {
  size_t data_size = 0;
  if (libregf_value_get_value_data_size(value, &data_size, nullptr) != 1 ||
      data_size == 0) {
    output.clear();
    return;
  }

  std::vector<uint8_t> buffer(data_size);
  if (libregf_value_get_value_utf8_string(value, buffer.data(), buffer.size(),
                                          nullptr) == 1) {
    output.assign(
        reinterpret_cast<const char*>(buffer.data()),
        strnlen(reinterpret_cast<const char*>(buffer.data()), data_size));
  } else {
    output.clear();
  }
}

void RegistryParser::Impl::extractDword(libregf_value_t* value,
                                        std::string& output) {
  uint32_t data = 0;
  if (libregf_value_get_value_32bit(value, &data, nullptr) == 1) {
    output = std::to_string(data);
  }
}

void RegistryParser::Impl::extractQword(libregf_value_t* value,
                                        std::string& output) {
  uint64_t data = 0;
  if (libregf_value_get_value_64bit(value, &data, nullptr) == 1) {
    output = std::to_string(data);
  }
}

void RegistryParser::Impl::extractBinary(libregf_value_t* value,
                                         std::string& output) {
  size_t data_size = 0;
  if (libregf_value_get_value_data_size(value, &data_size, nullptr) != 1 ||
      data_size == 0) {
    output.clear();
    return;
  }

  std::vector<uint8_t> buffer(data_size);
  if (libregf_value_get_value_data(value, buffer.data(), data_size, nullptr) !=
      1) {
    throw BinaryDataReadError();
  }

  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (size_t i = 0; i < data_size; ++i) {
    oss << std::setw(2) << static_cast<int>(buffer[i]);
    if (i != data_size - 1) {
      oss << " ";
    }
  }
  output = oss.str();
}

RegistryParser::RegistryParser() : impl_(std::make_unique<Impl>()) {}

RegistryParser::~RegistryParser() = default;

void RegistryParser::open(const std::string& file_path) const {
  impl_->open(file_path);
}

std::vector<RegistryValue> RegistryParser::getAllKeyValues(
    const std::string& key_path) const {
  return impl_->getAllKeyValues(key_path);
}

RegistryValue RegistryParser::getValueByName(
    const std::string& key_path, const std::string& value_name) const {
  return impl_->getValueByName(key_path, value_name);
}

}
