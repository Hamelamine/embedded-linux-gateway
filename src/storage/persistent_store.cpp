#include "gateway/storage/persistent_store.hpp"

#include <sqlite3.h>

#include <chrono>
#include <stdexcept>
#include <string>

namespace gateway {

namespace {

const char* sensor_type_to_string(SensorType type)
{
    switch (type) {
        case SensorType::Temperature:
            return "temperature";

        case SensorType::Vibration:
            return "vibration";

        case SensorType::MachineStatus:
            return "machine_status";
    }

    return "unknown";
}

bool string_to_sensor_type(
    const std::string& text,
    SensorType& type)
{
    if (text == "temperature") {
        type = SensorType::Temperature;
        return true;
    }

    if (text == "vibration") {
        type = SensorType::Vibration;
        return true;
    }

    if (text == "machine_status") {
        type = SensorType::MachineStatus;
        return true;
    }

    return false;
}

} // namespace

PersistentStore::PersistentStore(
    const std::string& database_path,
    std::size_t max_pending)
    : database_(nullptr),
      max_pending_(max_pending)
{
    const int result =
        sqlite3_open(
            database_path.c_str(),
            &database_
        );

    if (result != SQLITE_OK) {

        const std::string error_message =
            database_ != nullptr
                ? sqlite3_errmsg(database_)
                : "unknown SQLite error";

        if (database_ != nullptr) {
            sqlite3_close(database_);
            database_ = nullptr;
        }

        throw std::runtime_error(
            "Failed to open database: "
            + error_message
        );
    }

    if (!initialize_database()) {

        sqlite3_close(database_);
        database_ = nullptr;

        throw std::runtime_error(
            "Failed to initialize database schema"
        );
    }
}

PersistentStore::~PersistentStore()
{
    if (database_ != nullptr) {
        sqlite3_close(database_);
        database_ = nullptr;
    }
}

bool PersistentStore::initialize_database()
{
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS pending_measurements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sensor_id TEXT NOT NULL,
            sensor_type TEXT NOT NULL,
            value REAL NOT NULL,
            unit TEXT NOT NULL,
            timestamp_ms INTEGER NOT NULL
        );
    )";

    char* error_message = nullptr;

    const int result =
        sqlite3_exec(
            database_,
            sql,
            nullptr,
            nullptr,
            &error_message
        );

    if (result != SQLITE_OK) {

        if (error_message != nullptr) {
            sqlite3_free(error_message);
        }

        return false;
    }

    return true;
}

bool PersistentStore::store(
    const SensorReading& reading)
{
    if (pending_count() >= max_pending_) {
        return false;
    }

    const char* sql = R"(
        INSERT INTO pending_measurements (
            sensor_id,
            sensor_type,
            value,
            unit,
            timestamp_ms
        )
        VALUES (?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        return false;
    }

    const auto timestamp_ms =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                reading.timestamp.time_since_epoch()
        ).count();

    bool bind_ok = true;

    bind_ok &= sqlite3_bind_text(
        statement,
        1,
        reading.sensor_id.c_str(),
        -1,
        SQLITE_TRANSIENT
    ) == SQLITE_OK;

    bind_ok &= sqlite3_bind_text(
        statement,
        2,
        sensor_type_to_string(reading.type),
        -1,
        SQLITE_TRANSIENT
    ) == SQLITE_OK;

    bind_ok &= sqlite3_bind_double(
        statement,
        3,
        reading.value
    ) == SQLITE_OK;

    bind_ok &= sqlite3_bind_text(
        statement,
        4,
        reading.unit.c_str(),
        -1,
        SQLITE_TRANSIENT
    ) == SQLITE_OK;

    bind_ok &= sqlite3_bind_int64(
        statement,
        5,
        static_cast<sqlite3_int64>(
            timestamp_ms
        )
    ) == SQLITE_OK;

    if (!bind_ok) {
        sqlite3_finalize(statement);
        return false;
    }

    const int result =
        sqlite3_step(statement);

    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}

std::size_t PersistentStore::pending_count() const
{
    const char* sql =
        "SELECT COUNT(*) "
        "FROM pending_measurements;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        return 0;
    }

    std::size_t count = 0;

    if (sqlite3_step(statement) == SQLITE_ROW) {

        count =
            static_cast<std::size_t>(
                sqlite3_column_int64(
                    statement,
                    0
                )
            );
    }

    sqlite3_finalize(statement);

    return count;
}

std::optional<StoredMeasurement>
PersistentStore::load_oldest() const
{
    const char* sql = R"(
        SELECT
            id,
            sensor_id,
            sensor_type,
            value,
            unit,
            timestamp_ms
        FROM pending_measurements
        ORDER BY id ASC
        LIMIT 1;
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        return std::nullopt;
    }

    const int result =
        sqlite3_step(statement);

    if (result == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    if (result != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    const std::int64_t id =
        sqlite3_column_int64(
            statement,
            0
        );

    const char* sensor_id_text =
        reinterpret_cast<const char*>(
            sqlite3_column_text(
                statement,
                1
            )
        );

    const char* sensor_type_text =
        reinterpret_cast<const char*>(
            sqlite3_column_text(
                statement,
                2
            )
        );

    const double value =
        sqlite3_column_double(
            statement,
            3
        );

    const char* unit_text =
        reinterpret_cast<const char*>(
            sqlite3_column_text(
                statement,
                4
            )
        );

    const std::int64_t timestamp_ms =
        sqlite3_column_int64(
            statement,
            5
        );

    if (
        sensor_id_text == nullptr ||
        sensor_type_text == nullptr ||
        unit_text == nullptr
    ) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    SensorType sensor_type;

    if (!string_to_sensor_type(
            sensor_type_text,
            sensor_type)) {

        sqlite3_finalize(statement);
        return std::nullopt;
    }

    const auto duration =
        std::chrono::duration_cast<
            std::chrono::system_clock::duration>(
                std::chrono::milliseconds(
                    timestamp_ms
                )
        );

    const auto timestamp =
        std::chrono::system_clock::time_point(
            duration
        );

    StoredMeasurement stored{
        id,
        SensorReading{
            sensor_id_text,
            sensor_type,
            value,
            unit_text,
            timestamp
        }
    };

    sqlite3_finalize(statement);

    return stored;
}

bool PersistentStore::remove(
    std::int64_t id)
{
    const char* sql =
        "DELETE FROM pending_measurements "
        "WHERE id = ?;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &statement,
            nullptr) != SQLITE_OK) {

        return false;
    }

    if (sqlite3_bind_int64(
            statement,
            1,
            id) != SQLITE_OK) {

        sqlite3_finalize(statement);
        return false;
    }

    const int result =
        sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        return false;
    }

    return sqlite3_changes(database_) == 1;
}

} // namespace gateway