#include "gateway/logging/logger.hpp"

int main()
{
    gateway::log(
        gateway::LogLevel::Info,
        "Embedded Linux Industrial IoT Gateway starting"
    );

    gateway::log(
        gateway::LogLevel::Info,
        "Gateway initialized successfully"
    );

    return 0;
}