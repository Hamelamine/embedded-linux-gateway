#pragma once

#include <chrono>
#include <string>

namespace gateway {

enum class SensorType {
    Temperature,
    Vibration,
    MachineStatus
};

struct SensorReading {
    std::string sensor_id;
    SensorType type;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
};

class Sensor {
public:
    virtual ~Sensor() = default;

    virtual SensorReading read() = 0;
};

} // namespace gateway