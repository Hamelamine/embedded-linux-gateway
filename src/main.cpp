#include "gateway/logging/logger.hpp"
#include "gateway/sensors/simulated_sensor.hpp"

#include <chrono>
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

void log_reading(const gateway::SensorReading& reading)
{
    std::ostringstream message;

    message << "sensor=" << reading.sensor_id
            << " type=" << sensor_type_to_string(reading.type)
            << " value=" << reading.value
            << " unit=" << reading.unit;

    gateway::log(gateway::LogLevel::Info, message.str());
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

    const auto acquisition_interval = std::chrono::seconds(1);

    while (true) {
        log_reading(temperature_sensor.read());
        log_reading(vibration_sensor.read());
        log_reading(machine_status_sensor.read());

        std::this_thread::sleep_for(acquisition_interval);
    }

    return 0;
}