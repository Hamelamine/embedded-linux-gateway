#include "gateway/pipeline/reading_validator.hpp"

#include <chrono>
#include <iostream>

int main()
{
    gateway::ReadingValidator validator;

    // --------------------------------------------------
    // Temperature tests
    // --------------------------------------------------

    gateway::SensorReading normal_temperature{
        "temperature_01",
        gateway::SensorType::Temperature,
        25.0,
        "C",
        std::chrono::system_clock::now()
    };

    gateway::SensorReading bad_temperature{
        "temperature_01",
        gateway::SensorType::Temperature,
        300.0,
        "C",
        std::chrono::system_clock::now()
    };

    const auto normal_temperature_result =
        validator.validate(normal_temperature);

    const auto bad_temperature_result =
        validator.validate(bad_temperature);

    if (!normal_temperature_result.valid) {
        std::cerr << "FAIL: 25 C should be valid\n";
        return 1;
    }

    if (bad_temperature_result.valid) {
        std::cerr << "FAIL: 300 C should be rejected\n";
        return 1;
    }

    // --------------------------------------------------
    // Vibration tests
    // --------------------------------------------------

    gateway::SensorReading normal_vibration{
        "vibration_01",
        gateway::SensorType::Vibration,
        3.5,
        "mm/s",
        std::chrono::system_clock::now()
    };

    gateway::SensorReading bad_vibration{
        "vibration_01",
        gateway::SensorType::Vibration,
        -10.0,
        "mm/s",
        std::chrono::system_clock::now()
    };

    const auto normal_vibration_result =
        validator.validate(normal_vibration);

    const auto bad_vibration_result =
        validator.validate(bad_vibration);

    if (!normal_vibration_result.valid) {
        std::cerr << "FAIL: 3.5 mm/s should be valid\n";
        return 1;
    }

    if (bad_vibration_result.valid) {
        std::cerr << "FAIL: -10 mm/s should be rejected\n";
        return 1;
    }

    // --------------------------------------------------
    // Machine status tests
    // --------------------------------------------------

    gateway::SensorReading normal_machine_status{
        "machine_01",
        gateway::SensorType::MachineStatus,
        1.0,
        "state",
        std::chrono::system_clock::now()
    };

    gateway::SensorReading bad_machine_status{
        "machine_01",
        gateway::SensorType::MachineStatus,
        2.0,
        "state",
        std::chrono::system_clock::now()
    };

    const auto normal_machine_status_result =
        validator.validate(normal_machine_status);

    const auto bad_machine_status_result =
        validator.validate(bad_machine_status);

    if (!normal_machine_status_result.valid) {
        std::cerr << "FAIL: machine status 1 should be valid\n";
        return 1;
    }

    if (bad_machine_status_result.valid) {
        std::cerr << "FAIL: machine status 2 should be rejected\n";
        return 1;
    }

    std::cout << "PASS: all sensor validation rules work\n";

    return 0;
}