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

        // ==============================================
        // Instance #1
        // Store two measurements
        // ==============================================

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
            if (store.store(machine)) {
                std::cerr
                    << "FAIL: buffer should reject third measurement\n";

                return 1;
            }

            std::cout
                << "Buffer-full behavior verified\n";
            if (store.pending_count() != 2) {
                std::cerr
                    << "FAIL: expected 2 measurements\n";
                return 1;
            }

            std::cout
                << "Stored 2 measurements\n";
        }

        // ==============================================
        // Instance #2
        // Simulates reopening after restart
        // ==============================================

        {
            gateway::PersistentStore store(
                database_path,
                2
            );

            if (store.pending_count() != 2) {

                std::cerr
                    << "FAIL: measurements did not survive reopen\n";

                return 1;
            }

            auto oldest =
                store.load_oldest();

            if (!oldest.has_value()) {

                std::cerr
                    << "FAIL: oldest measurement not found\n";

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
                    << "FAIL: oldest measurement not removed\n";

                return 1;
            }

            if (store.pending_count() != 1) {

                std::cerr
                    << "FAIL: expected 1 measurement after delete\n";

                return 1;
            }

            oldest =
                store.load_oldest();

            if (!oldest.has_value()) {

                std::cerr
                    << "FAIL: second measurement not found\n";

                return 1;
            }

            if (
                oldest->reading.sensor_id !=
                "vibration_01"
            ) {

                std::cerr
                    << "FAIL: second measurement incorrect\n";

                return 1;
            }

            if (!store.remove(oldest->id)) {

                std::cerr
                    << "FAIL: second measurement not removed\n";

                return 1;
            }

            if (store.pending_count() != 0) {

                std::cerr
                    << "FAIL: database should now be empty\n";

                return 1;
            }

            const auto empty =
                store.load_oldest();

            if (empty.has_value()) {

                std::cerr
                    << "FAIL: expected empty queue\n";

                return 1;
            }
        }

        std::cout
            << "PASS: persistent FIFO queue works\n";

        return 0;

    } catch (const std::exception& error) {

        std::cerr
            << "FAIL: "
            << error.what()
            << '\n';

        return 1;
    }
}