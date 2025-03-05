#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../file_reader/file_reader.hpp"

enum class ValueType { REG_NONE, REG_SZ, REG_DWORD, REG_BINARY };

struct KeyNode {
  uint32_t offset;         //
  uint32_t subkeys_count;  //
  uint32_t values_count;   //
};

struct ValueEntry {
  ValueType type;        //
  uint32_t data_offset;  //
  uint32_t data_size;    //
  uint16_t name_length;  // Длина имени в символах UTF-16
  uint32_t name_offset;  // Смещение имени относительно ключа
};

class RegistryAnalysis {
 public:
  explicit RegistryAnalysis(const std::string&);
  std::vector<uint8_t> read_value(const std::string&, const std::string&,
                                  ValueType&) const;
  bool key_exists(const std::string&) const;

 private:
  void validate_hive();
  KeyNode parse_key_node(uint32_t) const;
  ValueEntry parse_value_entry(uint32_t) const;
  KeyNode find_key(const std::string&) const;
  bool compare_key_name(const KeyNode&, const std::string&) const;
  std::vector<uint8_t> read_value_data(const KeyNode&, const std::string&,
                                       ValueType&) ;
  bool RegistryAnalysis::compare_value_name(const ValueEntry&,
                                            const std::string&);

  FileReader reader_;
  uint32_t root_offset_;
};