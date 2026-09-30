#pragma once

#include "gateway/sensors/sensor.hpp"

#include <string>

namespace gateway {

struct ValidationResult {
    bool valid;
    std::string reason;
};

class ReadingValidator {
public:
    ValidationResult validate(const SensorReading& reading) const;
};

} // namespace gateway