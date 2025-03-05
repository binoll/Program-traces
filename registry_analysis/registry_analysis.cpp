#include "../registry_analysis/registry_analysis.hpp"
#include <codecvt>
#include <locale>
#include <stdexcept>

RegistryAnalysis::RegistryAnalysis(const std::string& hive_file)
    : reader_(hive_file, std::ios::binary | std::ios::in), root_offset_(0) {
  validate_hive();
}

std::vector<uint8_t> RegistryAnalysis::read_value(const std::string& key_path,
                                                  const std::string& value_name,
                                                  ValueType& out_type) const {
  const auto key_node = find_key(key_path);
  return read_value_data(key_node, value_name, out_type);
}

bool RegistryAnalysis::key_exists(const std::string& key_path) const {
  try {
    find_key(key_path);
    return true;
  } catch (...) {
    return false;
  }
}

void RegistryAnalysis::validate_hive() {
  constexpr size_t HIVE_SIGNATURE_SIZE = 4;
  const auto header = reader_.read_chunk(HIVE_SIGNATURE_SIZE);

  if (std::string(header.begin(), header.end()) != "regf") {
    throw std::runtime_error("Invalid registry hive format");
  }

  reader_.seek(0x24);
  const auto root_block = reader_.read_chunk(sizeof(uint32_t));
  root_offset_ = *reinterpret_cast<const uint32_t*>(root_block.data());
}

KeyNode RegistryAnalysis::parse_key_node(uint32_t offset) const {
  constexpr size_t KEY_NODE_HEADER_SIZE = 0x2C;
  reader_.seek(offset);
  const auto node_header = reader_.read_chunk(KEY_NODE_HEADER_SIZE);

  return KeyNode{
      .offset = offset,
      .subkeys_count = *reinterpret_cast<const uint32_t*>(&node_header[0x14]),
      .values_count = *reinterpret_cast<const uint32_t*>(&node_header[0x1C])};
}

ValueEntry RegistryAnalysis::parse_value_entry(uint32_t offset) const {
  constexpr size_t VALUE_HEADER_SIZE = 0x18;  // Размер заголовка значения

  reader_.seek(offset);
  const auto value_header = reader_.read_chunk(VALUE_HEADER_SIZE);

  return ValueEntry{
      .name_length = *reinterpret_cast<const uint16_t*>(&value_header[0x00]),
      .type = static_cast<ValueType>(
          *reinterpret_cast<const uint32_t*>(&value_header[0x04])),
      .data_offset = *reinterpret_cast<const uint32_t*>(&value_header[0x08]),
      .data_size = *reinterpret_cast<const uint32_t*>(&value_header[0x0C]),
      .name_offset = *reinterpret_cast<const uint32_t*>(&value_header[0x10])};
}

KeyNode RegistryAnalysis::find_key(const std::string& key_path) const {
  KeyNode current_node = parse_key_node(root_offset_);
  size_t start = 0;

  while (start < key_path.size()) {
    const auto pos = key_path.find('\\', start);
    const auto name = key_path.substr(start, pos - start);

    bool found = false;
    for (uint32_t i = 0; i < current_node.subkeys_count; ++i) {
      const auto subkey_node =
          parse_key_node(current_node.offset + 0x2C + i * 0x20);
      if (compare_key_name(subkey_node, name)) {
        current_node = subkey_node;
        found = true;
        break;
      }
    }

    if (!found)
      throw std::runtime_error("Key not found: " + name);
    start = pos != std::string::npos ? pos + 1 : key_path.size();
  }
  return current_node;
}

bool RegistryAnalysis::compare_key_name(const KeyNode& node,
                                        const std::string& name) const {
  reader_.seek(node.offset + 0x2C);
  const auto name_length = reader_.read_chunk(2);
  const uint16_t length =
      *reinterpret_cast<const uint16_t*>(name_length.data());

  const auto name_data = reader_.read_chunk(length);
  std::string key_name(name_data.begin(), name_data.end());

  return key_name == name;
}

bool RegistryAnalysis::compare_value_name(const ValueEntry& value_entry,
                                          const std::string& target_name) {
  // Если имя не указано (значение по умолчанию)
  if (value_entry.name_length == 0) {
    return target_name.empty();
  }

  // Читаем имя значения из файла реестра
  reader_.seek(value_entry.name_offset);
  const auto name_data =
      reader_.read_chunk(value_entry.name_length * 2);  // UTF-16

  // Конвертируем UTF-16LE в UTF-8
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  std::u16string u16_name(reinterpret_cast<const char16_t*>(name_data.data()),
                          value_entry.name_length);

  try {
    std::string utf8_name = converter.to_bytes(u16_name);
    return utf8_name == target_name;
  } catch (...) {
    return false;
  }
}

std::vector<uint8_t> RegistryAnalysis::read_value_data(
    const KeyNode& key_node, const std::string& value_name,
    ValueType& out_type)  {
  for (uint32_t i = 0; i < key_node.values_count; ++i) {
    const auto value_entry =
        parse_value_entry(key_node.offset + 0x2C + i * 0x20);

    if (compare_value_name(value_entry, value_name)) {
      reader_.seek(key_node.offset + 0x2C + value_entry.data_offset);
      const auto value_data = reader_.read_chunk(value_entry.data_size);

      out_type = value_entry.type;
      return std::vector<uint8_t>(value_data.begin(), value_data.end());
    }
  }
  throw std::runtime_error("Value not found: " + value_name);
}
