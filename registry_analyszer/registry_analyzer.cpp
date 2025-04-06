#include "registry_analyzer.hpp"
#include <iostream>
#include <memory>
#include <libregf.h>

RegistryAnalyzer::RegistryAnalyzer() : regf_file_(nullptr) {
    if (libregf_file_initialize(&regf_file_, nullptr) != 1) {
        throw RegistryInitializationError();
    }
}

RegistryAnalyzer::~RegistryAnalyzer() {
    if (regf_file_) {
        libregf_file_close(regf_file_, nullptr);
        libregf_file_free(&regf_file_, nullptr);
    }
}

void RegistryAnalyzer::OpenRegistryFile(const std::string& file_path) {
    if (libregf_file_open(
        regf_file_,
        file_path.c_str(),
        LIBREGF_ACCESS_FLAG_READ,
        nullptr) != 1) {
        throw FileOpenError(file_path);
    }
}

void RegistryAnalyzer::AnalyzeRootKeys() const {
    libregf_key_t* root_key = nullptr;
    if (libregf_file_get_root_key(
        regf_file_,
        &root_key,
        nullptr) != 1) {
        throw RootKeyError();
    }

    try {
        TraverseSubkeys(root_key);
        libregf_key_free(&root_key, nullptr);
    }
    catch (...) {
        libregf_key_free(&root_key, nullptr);
        throw;
    }
}

void RegistryAnalyzer::TraverseSubkeys(libregf_key_t* parent_key) const {
    int number_of_subkeys = 0;
    if (libregf_key_get_number_of_sub_keys(
        parent_key,
        &number_of_subkeys,
        nullptr) != 1) {
        return;
    }

    for (int i = 0; i < number_of_subkeys; ++i) {
        libregf_key_t* sub_key = nullptr;
        if (libregf_key_get_sub_key(
            parent_key,
            i,
            &sub_key,
            nullptr) == 1) {

            try {
                ProcessKey(sub_key);
                TraverseSubkeys(sub_key);
                libregf_key_free(&sub_key, nullptr);
            }
            catch (...) {
                libregf_key_free(&sub_key, nullptr);
                throw;
            }
        }
    }
}

void RegistryAnalyzer::ProcessKey(libregf_key_t* key) const {
    size_t name_size = 0;
    if (libregf_key_get_utf8_name_size(key, &name_size, nullptr) != 1) {
        return;
    }

    std::unique_ptr<char[]> buffer(new char[name_size]);
    if (libregf_key_get_utf8_name(
        key,
        reinterpret_cast<uint8_t*>(buffer.get()),
        name_size,
        nullptr) != 1) {
        return;
    }

    std::string key_name(buffer.get(), name_size - 1);
    std::cout << "Found key: " << key_name << std::endl;

    // Add additional key processing here
}
