
#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

#include <iostream>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk(Mavsdk::Configuration{ComponentType::CompanionComputer});
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cout << "Adding connection failed: " << connection_result << '\n';
        return 1;
    } else {
        std::cout << "Connection successful\n";
    }

    // Wait for the system to connect via heartbeat
    while (mavsdk.systems().size() == 0) {
    sleep_for(seconds(1));
    }
    // System got discovered.
    auto system = mavsdk.systems().at(0);
    Telemetry telemetry{system};
    Action action{system};
    Offboard offboard{system};

    std::cout << "Waiting for vehicle to be ready...\n";
    while (!telemetry.health_all_ok()) {
        sleep_for(seconds(1));
        std::cout << "Vehicle is getting ready\n" << telemetry.health();
        if (telemetry.health().is_armable) {
            std::cout << "Vehicle is armable\n";
            break;
        }
    }

    std::cout << "Vehicle is ready\n";

    std::cout << "Arming vehicle...\n";
    Action::Result arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }

    // **Send initial setpoints before starting offboard mode**
    std::cout << "Sending initial setpoints...\n";
    offboard.set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
    sleep_for(seconds(1));
    offboard.set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f}); // Send twice

    std::cout << "Starting Offboard mode...\n";
    Offboard::Result offboard_result = offboard.start();
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard mode start failed: " << offboard_result << '\n';
        return 1;
    }

    offboard_result = offboard.set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
    }

    offboard_result = offboard.start();
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
    }

    offboard.set_velocity_ned({0.0f, 0.0f, -2.0f, 0.0f});
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
    }

    sleep_for(seconds(5));

    offboard_result = offboard.set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
    }

    sleep_for(seconds(5));

    offboard_result = offboard.set_velocity_ned({0.0f, 0.0f, 2.0f, 0.0f});
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
    }

    sleep_for(seconds(5));

    offboard.stop();

    std::cout << "Finished mission\n";

    return 0;
}