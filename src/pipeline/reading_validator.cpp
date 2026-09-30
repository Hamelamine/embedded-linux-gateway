#include "gateway/pipeline/reading_validator.hpp"

namespace gateway {

ValidationResult ReadingValidator::validate(
    const SensorReading& reading) const
{
    if (reading.sensor_id.empty()) {
        return {
            false,
            "sensor_id is empty"
        };
    }

    if (reading.type == SensorType::Temperature) {
        if (reading.value < -40.0 || reading.value > 125.0) {
            return {
                false,
                "temperature outside valid range"
            };
        }
    }

    if (reading.type == SensorType::Vibration) {
        if (reading.value < 0.0 || reading.value > 100.0) {
            return {
                false,
                "vibration outside valid range"
            };
        }
    }

    if (reading.type == SensorType::MachineStatus) {
        if (reading.value != 0.0 && reading.value != 1.0) {
            return {
                false,
                "machine status must be 0 or 1"
            };
        }
    }

    return {
        true,
        ""
    };
}

} // namespace gateway