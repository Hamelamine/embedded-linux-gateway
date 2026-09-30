cmake_minimum_required(VERSION 3.16)

project(
    embedded_linux_gateway
    VERSION 0.1.0
    LANGUAGES CXX
)

# --------------------------------------------------
# C++ configuration
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# External dependencies
# --------------------------------------------------

find_package(SQLite3 REQUIRED)

# ==================================================
# Main gateway application
# ==================================================

add_executable(gateway
    src/main.cpp
    src/logging/logger.cpp
    src/sensors/simulated_sensor.cpp
    src/pipeline/reading_validator.cpp
    src/storage/persistent_store.cpp
)

target_include_directories(gateway
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(gateway
    PRIVATE
        SQLite::SQLite3
)

target_compile_options(gateway
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ==================================================
# Reading validator test
# ==================================================

add_executable(validator_test
    tests/reading_validator_test.cpp
    src/pipeline/reading_validator.cpp
)

target_include_directories(validator_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(validator_test
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ==================================================
# Persistent storage test
# ==================================================

add_executable(persistent_store_test
    tests/persistent_store_test.cpp
    src/storage/persistent_store.cpp
)

target_include_directories(persistent_store_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(persistent_store_test
    PRIVATE
        SQLite::SQLite3
)

target_compile_options(persistent_store_test
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)