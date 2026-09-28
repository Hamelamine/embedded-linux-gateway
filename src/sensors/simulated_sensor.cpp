#include "gateway/sensors/simulated_sensor.hpp"

#include <chrono>
#include <random>
#include <stdexcept>
#include <utility>

namespace gateway {

SimulatedSensor::SimulatedSensor(
    std::string sensor_id,
    SensorType type
)
    : sensor_id_(std::move(sensor_id)),
      type_(type),
      random_generator_(std::random_device{}())
{
}

SensorReading SimulatedSensor::read()
{
    return SensorReading{
        sensor_id_,
        type_,
        generate_value(),
        unit_for_type(),
        std::chrono::system_clock::now()
    };
}

double SimulatedSensor::generate_value()
{
    switch (type_) {
        case SensorType::Temperature: {
            std::uniform_real_distribution<double> distribution(20.0, 35.0);
            return distribution(random_generator_);
        }

        case SensorType::Vibration: {
            std::uniform_real_distribution<double> distribution(0.1, 5.0);
            return distribution(random_generator_);
        }

        case SensorType::MachineStatus: {
            std::bernoulli_distribution distribution(0.9);
            return distribution(random_generator_) ? 1.0 : 0.0;
        }
    }

    throw std::runtime_error("Unsupported sensor type");
}

std::string SimulatedSensor::unit_for_type() const
{
    switch (type_) {
        case SensorType::Temperature:
            return "C";

        case SensorType::Vibration:
            return "mm/s";

        case SensorType::MachineStatus:
            return "state";
    }

    return "unknown";
}

} // namespace gateway