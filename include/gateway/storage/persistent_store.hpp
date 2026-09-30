#pragma once

#include "gateway/sensors/sensor.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

struct sqlite3;

namespace gateway {

struct StoredMeasurement {
    std::int64_t id;
    SensorReading reading;
};

class PersistentStore {
public:
    PersistentStore(
        const std::string& database_path,
        std::size_t max_pending
    );

    ~PersistentStore();

    PersistentStore(const PersistentStore&) = delete;

    PersistentStore& operator=(
        const PersistentStore&
    ) = delete;

    bool store(const SensorReading& reading);

    std::size_t pending_count() const;

    std::optional<StoredMeasurement>
    load_oldest() const;

    bool remove(std::int64_t id);

private:
    bool initialize_database();

    sqlite3* database_;

    std::size_t max_pending_;
};

} // namespace gateway