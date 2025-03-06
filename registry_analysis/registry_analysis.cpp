#include "registry_analysis.hpp"
#include <codecvt>
#include <locale>
#include "../file_reader/file_reader_exceptions.hpp"
#include "registry_analysis_exceptions.hpp"

namespace {
constexpr uint32_t HIVE_BASE_OFFSET = RegistryEntry::HIVE_BASE_OFFSET;
constexpr uint16_t MAX_VALUE_SIZE = RegistryEntry::MAX_VALUE_SIZE;
constexpr size_t HIVE_HEADER_SIZE = RegistryEntry::HIVE_HEADER_SIZE;
constexpr std::string_view HIVE_SIGNATURE = RegistryEntry::HIVE_SIGNATURE;
constexpr size_t HIVE_VERSION_OFFSET = 0x08;
constexpr size_t ROOT_KEY_OFFSET = 0x24;
constexpr char PATH_DELIMITER = '\\';
}  // namespace

RegistryAnalyzer::RegistryAnalyzer(const std::string& hive_path)
    : reader_(std::make_unique<FileReader>(hive_path)),
      root_key_offset_(0),
      hive_version_(0),
      is_valid_hive_(false) {
  try {
    validate_hive_header();
    is_valid_hive_ = true;
  } catch (const HiveValidationError&) {
    is_valid_hive_ = false;
    throw;
  } catch (...) {
    is_valid_hive_ = false;
    throw HiveValidationError("Неизвестная ошибка инициализации");
  }
}

RegistryAnalyzer::~RegistryAnalyzer() = default;

std::vector<uint8_t> RegistryAnalyzer::read_value(const std::string& key_path,
                                                  const std::string& value_name,
                                                  ValueType& out_type) const {
  if (!is_valid_hive_) {
    throw HiveValidationError("Файл HIVE не инициализирован");
  }

  const KeyNode target_key = traverse_key_path(key_path);

  for (uint32_t i = 0; i < target_key.values_count; ++i) {
    const uint32_t entry_offset =
        target_key.offset + sizeof(KeyNode) + i * sizeof(ValueEntry);

    const ValueEntry entry = parse_value_header(entry_offset);
    const std::string current_name = decode_utf16_name(entry);

    if (current_name == value_name) {
      out_type = entry.type;
      return read_value_content(entry);
    }
  }

  throw ValueNotFoundError(value_name);
}

bool RegistryAnalyzer::key_exists(const std::string& key_path) const noexcept {
  try {
    traverse_key_path(key_path);
    return true;
  } catch (...) {
    return false;
  }
}

std::string RegistryAnalyzer::hive_version() const {
  const auto major = static_cast<uint16_t>(hive_version_ >> 16);
  const auto minor = static_cast<uint16_t>(hive_version_ & 0xFFFF);
  return std::to_string(major) + "." + std::to_string(minor);
}

bool RegistryAnalyzer::is_loaded() const noexcept {
  return is_valid_hive_;
}

void RegistryAnalyzer::validate_hive_header() {
  const auto header =
      reader_->read_chunk(static_cast<std::streamsize>(HIVE_HEADER_SIZE));

  if (header.size() < HIVE_HEADER_SIZE) {
    throw HiveValidationError("Несоответствие размера заголовка");
  }

  if (!std::equal(HIVE_SIGNATURE.begin(), HIVE_SIGNATURE.end(),
                  header.begin())) {
    throw HiveValidationError("Неверная подпись файла");
  }

  hive_version_ =
      *reinterpret_cast<const uint32_t*>(header.data() + HIVE_VERSION_OFFSET);
  if ((hive_version_ >> 16) != 1) {
    throw HiveValidationError("Неподдерживаемая версия: " + hive_version());
  }

  root_key_offset_ =
      *reinterpret_cast<const uint32_t*>(header.data() + ROOT_KEY_OFFSET) +
      HIVE_BASE_OFFSET;
}

KeyNode RegistryAnalyzer::parse_key_header(uint32_t offset) const {
  reader_->seek(offset);
  const auto data = reader_->read_chunk(sizeof(KeyNode));

  if (data.size() != sizeof(KeyNode)) {
    throw FileReadException(reader_->path(),
                            "Недопустимый размер ключевого заголовка");
  }

  return *reinterpret_cast<const KeyNode*>(data.data());
}

ValueEntry RegistryAnalyzer::parse_value_header(uint32_t offset) const {
  reader_->seek(offset);
  const auto data = reader_->read_chunk(sizeof(ValueEntry));

  if (data.size() != sizeof(ValueEntry)) {
    throw FileReadException(reader_->path(),
                            "Недопустимое значение размера заголовка");
  }

  return *reinterpret_cast<const ValueEntry*>(data.data());
}

KeyNode RegistryAnalyzer::traverse_key_path(const std::string& key_path) const {
  KeyNode current_node = parse_key_header(root_key_offset_);
  size_t start_pos = 0;

  while (start_pos < key_path.size()) {
    const size_t delimiter_pos = key_path.find(PATH_DELIMITER, start_pos);
    const std::string segment =
        key_path.substr(start_pos, delimiter_pos - start_pos);

    bool found = false;
    for (uint32_t i = 0; i < current_node.subkeys_count; ++i) {
      const uint32_t subkey_offset =
          current_node.offset + sizeof(KeyNode) +
          current_node.values_count * sizeof(ValueEntry) + i * sizeof(KeyNode);

      if (compare_key_name(subkey_offset, segment)) {
        current_node = parse_key_header(subkey_offset);
        found = true;
        break;
      }
    }

    if (!found) {
      throw KeyNotFoundError(segment);
    }
    start_pos = (delimiter_pos != std::string::npos) ? delimiter_pos + 1
                                                     : key_path.size();
  }

  return current_node;
}

bool RegistryAnalyzer::compare_key_name(uint32_t key_offset,
                                        const std::string& target_name) const {
  try {
    const ValueEntry name_entry = parse_value_header(key_offset);
    return decode_utf16_name(name_entry) == target_name;
  } catch (...) {
    return false;
  }
}

std::vector<uint8_t> RegistryAnalyzer::read_value_content(
    const ValueEntry& entry) const {
  if (entry.data_size > MAX_VALUE_SIZE) {
    throw ValueNotFoundError("Размер значения превышает установленный предел");
  }

  reader_->seek(entry.data_offset);
  const auto data =
      reader_->read_chunk(static_cast<std::streamsize>(entry.data_size));

  return {data.begin(), data.end()};
}

std::string RegistryAnalyzer::decode_utf16_name(const ValueEntry& entry) const {
  if (entry.name_length == 0) {
    return {};
  }

  const size_t byte_size = entry.name_length * sizeof(char16_t);
  reader_->seek(entry.name_offset);
  const auto buffer =
      reader_->read_chunk(static_cast<std::streamsize>(byte_size));

  if (buffer.size() != byte_size) {
    throw EncodingError("Неполное декодирование UTF-16");
  }

  try {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.to_bytes(
        reinterpret_cast<const char16_t*>(buffer.data()),
        reinterpret_cast<const char16_t*>(buffer.data() + buffer.size()));
  } catch (...) {
    throw EncodingError("Не удалось выполнить преобразование в UTF-16");
  }
}
