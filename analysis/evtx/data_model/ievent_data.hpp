#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace EventLogAnalysis {

enum class EventLevel {
    LOG_ALWAYS = 0,
    CRITICAL = 1,
    ERROR = 2,
    WARNING = 3,
    INFO = 4,
    VERBOSE = 5
};

/// @brief Интерфейс для данных события
class IEventData {
public:
    virtual ~IEventData() = default;

    virtual uint32_t EventID() const = 0;
    virtual uint64_t Timestamp() const = 0;  // Возвращаем FILETIME
    virtual EventLevel Level() const = 0;
    virtual const std::string& Provider() const = 0;
    virtual const std::string& Computer() const = 0;
    virtual const std::string& Channel() const = 0;
    virtual const std::string& Description() const = 0;
    virtual const std::unordered_map<std::string, std::string>& Data() const = 0;
    virtual const std::string& XmlRepresentation() const = 0;
};



} // namespace EventLogAnalysis