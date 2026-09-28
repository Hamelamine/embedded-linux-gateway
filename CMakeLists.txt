cmake_minimum_required(VERSION 3.16)

project(
    embedded_linux_gateway
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(gateway
    src/main.cpp
    src/logging/logger.cpp
    src/sensors/simulated_sensor.cpp
)

target_include_directories(gateway
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(gateway
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)