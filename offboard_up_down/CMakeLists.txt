cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(offboard_up_down)

add_executable(offboard_up_down
    offboard_up_down.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(offboard_up_down
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(offboard_up_down PRIVATE -Wall -Wextra)
else()
    add_compile_options(offboard_up_down PRIVATE -W2)
endif()
