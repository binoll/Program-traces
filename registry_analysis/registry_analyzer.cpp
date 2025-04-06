#include "registry_analyzer.hpp"
#include <iostream>
#include <libregf.h>

RegistryAnalyzer::RegistryAnalyzer() : regf_file_(nullptr) {
    if (libregf_file_initialize(&regf_file_, nullptr) != 1) {
        throw std::runtime_error("Failed to initialize REGf file");
    }
}

RegistryAnalyzer::~RegistryAnalyzer() {
    if (regf_file_) {
        libregf_file_close(regf_file_, nullptr);
        libregf_file_free(&regf_file_, nullptr);
    }
}

void RegistryAnalyzer::OpenRegistryFile(const std::string& file_path) {
    if (regf_file_open(regf_file_, file_path.c_str(), REGF_FILE_OPEN_READ, nullptr) != 1) {
        throw std::runtime_error("Failed to open registry file: " + file_path);
    }
}

void RegistryAnalyzer::AnalyzeRootKeys() const {
    libregf_key_t* root_key = nullptr;
    if (regf_file_get_root_key(regf_file_, &root_key, 0, nullptr) != 1) {
        throw std::runtime_error("Failed to get root key");
    }

    TraverseSubkeys(root_key);
    libregf_key_free(&root_key, nullptr);
}

void RegistryAnalyzer::TraverseSubkeys(libregf_key_t* parent_key) const {
    libregf_key_t* sub_key = nullptr;
    int number_of_subkeys = 0;

    if (libregf_key_get_number_of_sub_keys(parent_key, &number_of_subkeys, nullptr) != 1) {
        return;
    }

    for (int i = 0; i < number_of_subkeys; ++i) {
        if (regf_key_get_sub_key(parent_key, i, &sub_key, 0, nullptr) == 1) {
            ProcessKey(sub_key);
            TraverseSubkeys(sub_key); // Рекурсивный обход
            libregf_key_free(&sub_key, nullptr);
        }
    }
}

void RegistryAnalyzer::ProcessKey(libregf_key_t* key) const {
    std::string key_name;
    size_t name_size = 0;

    if (libregf_key_get_utf8_name_size(key, &name_size, nullptr) == 1) {
        char* buffer = new char[name_size];
        if (libregf_key_get_utf8_name(key, (uint8_t*)buffer, name_size, nullptr) == 1) {
            key_name.assign(buffer, name_size - 1);
        }
        delete[] buffer;
    }

    // Пример анализа: вывод имени ключа
    std::cout << "Found key: " << key_name << std::endl;

    // Здесь можно добавить анализ значений ключа
}
