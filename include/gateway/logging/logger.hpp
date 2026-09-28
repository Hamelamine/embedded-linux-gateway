#pragma once

#include <string>

namespace gateway {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

void log(LogLevel level, const std::string& message);

} // namespace gateway