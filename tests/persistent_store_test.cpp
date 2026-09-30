#include "gateway/storage/persistent_store.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>

int main()
{
    const std::string database_path =
        "data/test_gateway.db";

    try {

        {
            gateway::PersistentStore store(
                database_path,
                2
            );

            gateway::SensorReading temperature{
                "temperature_01",
                gateway::SensorType::Temperature,
                27.5,
                "C",
                std::chrono::system_clock::now()
            };

            gateway::SensorReading vibration{
                "vibration_01",
                gateway::SensorType::Vibration,
                3.2,
                "mm/s",
                std::chrono::system_clock::now()
            };

            gateway::SensorReading machine{
                "machine_01",
                gateway::SensorType::MachineStatus,
                1.0,
                "state",
                std::chrono::system_clock::now()
            };

            if (!store.store(temperature)) {
                std::cerr
                    << "FAIL: temperature not stored\n";
                return 1;
            }

            if (!store.store(vibration)) {
                std::cerr
                    << "FAIL: vibration not stored\n";
                return 1;
            }

            if (store.pending_count() != 2) {
                std::cerr
                    << "FAIL: expected 2 measurements\n";
                return 1;
            }

            if (store.store(machine)) {
                std::cerr
                    << "FAIL: full buffer accepted third measurement\n";
                return 1;
            }

            if (store.pending_count() != 2) {
                std::cerr
                    << "FAIL: count changed after buffer rejection\n";
                return 1;
            }

            std::cout
                << "Buffer-full behavior verified\n";
        }

        // Simulate reopening after restart.
        {
            gateway::PersistentStore store(
                database_path,
                2
            );

            if (store.pending_count() != 2) {
                std::cerr
                    << "FAIL: persistence after reopen failed\n";
                return 1;
            }

            auto oldest =
                store.load_oldest();

            if (!oldest.has_value()) {
                std::cerr
                    << "FAIL: oldest measurement missing\n";
                return 1;
            }

            if (
                oldest->reading.sensor_id !=
                "temperature_01"
            ) {
                std::cerr
                    << "FAIL: FIFO order incorrect\n";
                return 1;
            }

            std::cout
                << "Oldest measurement: "
                << oldest->reading.sensor_id
                << '\n';

            if (!store.remove(oldest->id)) {
                std::cerr
                    << "FAIL: first measurement not removed\n";
                return 1;
            }

            oldest = store.load_oldest();

            if (
                !oldest.has_value() ||
                oldest->reading.sensor_id !=
                    "vibration_01"
            ) {
                std::cerr
                    << "FAIL: second FIFO measurement incorrect\n";
                return 1;
            }

            if (!store.remove(oldest->id)) {
                std::cerr
                    << "FAIL: second measurement not removed\n";
                return 1;
            }

            if (store.pending_count() != 0) {
                std::cerr
                    << "FAIL: queue should be empty\n";
                return 1;
            }

            if (store.load_oldest().has_value()) {
                std::cerr
                    << "FAIL: empty queue returned a measurement\n";
                return 1;
            }
        }

        std::cout
            << "PASS: persistent bounded FIFO queue works\n";

        return 0;

    } catch (const std::exception& error) {

        std::cerr
            << "FAIL: "
            << error.what()
            << '\n';

        return 1;
    }
}