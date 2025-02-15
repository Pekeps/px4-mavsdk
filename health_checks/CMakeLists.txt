cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(health_checks)

add_executable(health_checks
    health_checks.cpp
    telemetry_logger.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(health_checks
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(offboard_up_down PRIVATE -Wall -Wextra)
else()
    add_compile_options(offboard_up_down PRIVATE -W2)
endif()
