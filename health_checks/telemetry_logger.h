#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <queue>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <thread>
#include <condition_variable>

class TelemetryLogger {
public:
    explicit TelemetryLogger(const std::string& log_filename);
    ~TelemetryLogger();

    void log_all(mavsdk::Telemetry& telemetry);

    void log_position(const mavsdk::Telemetry::PositionVelocityNed& position);
    void log_battery(const mavsdk::Telemetry::Battery& battery);
    void log_health(const mavsdk::Telemetry::Health& health);
    void log_status_text(const std::string& text);

    

private:
    std::ofstream log_file_;
    std::mutex queue_mutex_;
    std::queue<std::string> log_queue_;
    std::thread log_thread_;
    std::condition_variable log_condition_;

    std::optional<mavsdk::Telemetry::Health> prev_health_;
    bool stop_logging_ = false;

    void log_worker(); 
    void enqueue_log(const std::string& message);
    std::string format_double(double value, int precision = 6);
    std::string current_time();
};
