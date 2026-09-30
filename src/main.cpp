#include "gateway/logging/logger.hpp"
#include "gateway/pipeline/reading_validator.hpp"
#include "gateway/sensors/simulated_sensor.hpp"

#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

namespace {

std::string sensor_type_to_string(gateway::SensorType type)
{
    switch (type) {
        case gateway::SensorType::Temperature:
            return "temperature";

        case gateway::SensorType::Vibration:
            return "vibration";

        case gateway::SensorType::MachineStatus:
            return "machine_status";
    }

    return "unknown";
}

std::string timestamp_to_iso8601(
    const std::chrono::system_clock::time_point& timestamp)
{
    const std::time_t time =
        std::chrono::system_clock::to_time_t(timestamp);

    std::tm utc_time{};
    gmtime_r(&time, &utc_time);

    std::ostringstream stream;

    stream << std::put_time(
        &utc_time,
        "%Y-%m-%dT%H:%M:%SZ"
    );

    return stream.str();
}

void log_reading(const gateway::SensorReading& reading)
{
    std::ostringstream message;

    message << "timestamp="
            << timestamp_to_iso8601(reading.timestamp)
            << " sensor=" << reading.sensor_id
            << " type=" << sensor_type_to_string(reading.type)
            << " value=" << reading.value
            << " unit=" << reading.unit;

    gateway::log(
        gateway::LogLevel::Info,
        message.str()
    );
}

} // namespace

int main()
{
    gateway::log(
        gateway::LogLevel::Info,
        "Embedded Linux Industrial IoT Gateway starting"
    );

    gateway::SimulatedSensor temperature_sensor(
        "temperature_01",
        gateway::SensorType::Temperature
    );

    gateway::SimulatedSensor vibration_sensor(
        "vibration_01",
        gateway::SensorType::Vibration
    );

    gateway::SimulatedSensor machine_status_sensor(
        "machine_01",
        gateway::SensorType::MachineStatus
    );

    gateway::ReadingValidator validator;

    const auto acquisition_interval =
        std::chrono::seconds(1);

    while (true) {

        const std::array<gateway::SensorReading, 3> readings{
            temperature_sensor.read(),
            vibration_sensor.read(),
            machine_status_sensor.read()
        };

        for (const auto& reading : readings) {

            const gateway::ValidationResult result =
                validator.validate(reading);

            if (result.valid) {

                log_reading(reading);

            } else {

                gateway::log(
                    gateway::LogLevel::Warning,
                    "Rejected reading sensor=" +
                        reading.sensor_id +
                        " reason=" +
                        result.reason
                );
            }
        }

        std::this_thread::sleep_for(
            acquisition_interval
        );
    }

    return 0;
}