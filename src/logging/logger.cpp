#include "gateway/logging/logger.hpp"

#include <iostream>

namespace gateway {

namespace {

const char* level_to_string(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";

        case LogLevel::Info:
            return "INFO";

        case LogLevel::Warning:
            return "WARN";

        case LogLevel::Error:
            return "ERROR";
    }

    return "UNKNOWN";
}

} // namespace

void log(LogLevel level, const std::string& message)
{
    std::cout << "[" << level_to_string(level) << "] "
              << message << '\n';
}

} // namespace gateway