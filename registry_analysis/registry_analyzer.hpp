/**
 * @file registry_analyzer.hpp
 * @brief Windows Registry Analyzer using libregf
 * @author Your Name
 * @date 2023-09-20
 */

#pragma once

#include <libregf.h>
#include <string>
#include <stdexcept>

/**
 * @defgroup Exceptions Custom Exceptions
 * @brief Custom exception classes for registry analysis errors
 * @{
 */

/**
 * @class RegistryException
 * @brief Base exception class for registry-related errors
 */
class RegistryException : public std::runtime_error {
public:
    explicit RegistryException(const std::string& msg)
        : std::runtime_error(msg) {}
};

/**
 * @class RegistryInitializationError
 * @brief Exception thrown when registry file initialization fails
 */
class RegistryInitializationError : public RegistryException {
public:
    RegistryInitializationError()
        : RegistryException("Failed to initialize registry file") {}
};

/**
 * @class FileOpenError
 * @brief Exception thrown when file opening fails
 */
class FileOpenError : public RegistryException {
public:
    explicit FileOpenError(const std::string& path)
        : RegistryException("Failed to open registry file: " + path) {}
};

/**
 * @class RootKeyError
 * @brief Exception thrown when root key retrieval fails
 */
class RootKeyError : public RegistryException {
public:
    RootKeyError()
        : RegistryException("Failed to get root key") {}
};
/** @} */ // end of Exceptions group

/**
 * @class RegistryAnalyzer
 * @brief Main class for Windows registry file analysis
 *
 * Provides functionality to open and analyze Windows registry files
 * using libregf library. Supports recursive traversal of registry keys.
 */
class RegistryAnalyzer {
public:
    /**
     * @brief Default constructor
     * @throw RegistryInitializationError if initialization fails
     */
    RegistryAnalyzer();

    /**
     * @brief Destructor
     */
    ~RegistryAnalyzer();

    /**
     * @brief Open registry file
     * @param file_path Path to registry file
     * @throw FileOpenError if file cannot be opened
     */
    void OpenRegistryFile(const std::string& file_path);

    /**
     * @brief Analyze root keys
     * @throw RootKeyError if root key cannot be retrieved
     */
    void AnalyzeRootKeys() const;

private:
    libregf_file_t* regf_file_; ///< libregf file handle

    /**
     * @brief Recursively traverse subkeys
     * @param parent_key Parent key to start traversal from
     */
    void TraverseSubkeys(libregf_key_t* parent_key) const;

    /**
     * @brief Process individual registry key
     * @param key Key to process
     */
    void ProcessKey(libregf_key_t* key) const;
};
