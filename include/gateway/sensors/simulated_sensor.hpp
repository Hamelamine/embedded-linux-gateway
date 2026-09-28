#pragma once

#include "gateway/sensors/sensor.hpp"

#include <random>
#include <string>

namespace gateway {

class SimulatedSensor : public Sensor {
public:
    SimulatedSensor(std::string sensor_id, SensorType type);

    SensorReading read() override;

private:
    double generate_value();
    std::string unit_for_type() const;

    std::string sensor_id_;
    SensorType type_;
    std::mt19937 random_generator_;
};

} // namespace gateway