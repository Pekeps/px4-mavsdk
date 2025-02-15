#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
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
#include <fstream>

#include "telemetry_logger.h"


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

bool preflight_checks(Telemetry& telemetry)
{
    Telemetry::Battery battery = telemetry.battery();
    if (battery.remaining_percent < 40.0f) {
        std::cerr << "Battery is low (" << battery.remaining_percent << "), aborting\n";
        return false;
    }

    Telemetry::Health health = telemetry.health();
    if (!health.is_gyrometer_calibration_ok) {
        std::cerr << "Preflight check failed: Gyrometer calibration not ok\n";
        return false;
    }
    if (!health.is_accelerometer_calibration_ok) {
        std::cerr << "Preflight check failed: Accelerometer calibration not ok\n";
        return false;
    }
    if (!health.is_magnetometer_calibration_ok) {
        std::cerr << "Preflight check failed: Magnetometer calibration not ok\n";
        return false;
    }
    if (!health.is_local_position_ok) {
        std::cerr << "Preflight check failed: Local position not ok\n";
        return false;
    }
    // if (!health.is_global_position_ok) {
    //     std::cerr << "Preflight check failed: Global position not ok\n";
    //     return false;
    // }
    if (!health.is_home_position_ok) {
        std::cerr << "Preflight check failed: Home position not ok\n";
        return false;
    }
    if (!health.is_armable) {
        std::cerr << "Preflight check failed: Vehicle not armable\n";
        return false;
    }

    return true;
};

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>


int main(int argc, char** argv)
{

     if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::CompanionComputer}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cout << "Adding connection failed: " << connection_result << '\n';
        return 1;
    } else {
        std::cout << "Connection successful\n";
    }

    while (mavsdk.systems().size() == 0) {
        sleep_for(seconds(1));
    }

    auto system = mavsdk.systems().at(0);

    Telemetry telemetry{system};
    Action action{system};
    Offboard offboard{system};

    TelemetryLogger logger("telemetry_log.txt");
    logger.log_all(telemetry);

    while (!preflight_checks(telemetry)) {
        std::cout << "Preflight checks failed, waiting 5 seconds\n";
        sleep_for(seconds(5));
    }

    std::cout << "Preflight checks passed\n";
    

    sleep_for(seconds(5));

};
