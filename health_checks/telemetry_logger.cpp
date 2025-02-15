#include "telemetry_logger.h"

using namespace mavsdk;

TelemetryLogger::TelemetryLogger(const std::string& log_filename) {
    log_file_.open(log_filename, std::ios::out | std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << log_filename << std::endl;
    } else {
        log_file_ << "---- MAVSDK Telemetry Log ----\n";
    }

    // Start the log worker thread
    log_thread_ = std::thread(&TelemetryLogger::log_worker, this);
}

TelemetryLogger::~TelemetryLogger() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_logging_ = true;
    }
    log_condition_.notify_one(); // Wake up the logging thread
    log_thread_.join(); // Wait for thread to finish

    if (log_file_.is_open()) {
        log_file_ << "---- Logging Stopped ----\n";
        log_file_.close();
    }
}

void TelemetryLogger::log_worker() {
    while (true) {
        std::string log_message;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            log_condition_.wait(lock, [this]() {
                return !log_queue_.empty() || stop_logging_;
            });

            if (stop_logging_ && log_queue_.empty()) {
                return;
            }

            log_message = std::move(log_queue_.front());
            log_queue_.pop();
        }

        if (log_file_.is_open()) {
            log_file_ << log_message << std::endl;
        }
    }
}

void TelemetryLogger::enqueue_log(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        log_queue_.push(message);
    }
    log_condition_.notify_one();
}

void TelemetryLogger::log_battery(const Telemetry::Battery& battery) {
    enqueue_log("[Battery] Remaining: " + format_double(battery.remaining_percent) + "%" +
    ", " + "Voltage: " + format_double(battery.voltage_v) + "V");
}

void TelemetryLogger::log_position(const Telemetry::PositionVelocityNed& position) {
    enqueue_log("[Position] N: " + format_double(position.position.north_m) + "m, " +
    "E: " + format_double(position.position.east_m) + "m, " +
    "D: " + format_double(position.position.down_m) + "m");
}

void TelemetryLogger::log_status_text(const std::string& text) {
    enqueue_log("[Status] " + text);
}

void TelemetryLogger::log_health(const mavsdk::Telemetry::Health& health) {
    // Check if health state changed before logging
    if (prev_health_ && *prev_health_ == health) {
        return; // No change, skip logging
    }

    prev_health_ = health; // Update previous health state

    // Create health status message
    std::ostringstream oss;
    oss << "[Health] Gyro: " << (health.is_gyrometer_calibration_ok ? "OK" : "NOT OK") << ", "
        << "Accel: " << (health.is_accelerometer_calibration_ok ? "OK" : "NOT OK") << ", "
        << "Mag: " << (health.is_magnetometer_calibration_ok ? "OK" : "NOT OK") << ", "
        << "Local Pos: " << (health.is_local_position_ok ? "OK" : "NOT OK") << ", "
        << "Global Pos: " << (health.is_global_position_ok ? "OK" : "NOT OK") << ", "
        << "Home Pos: " << (health.is_home_position_ok ? "OK" : "NOT OK") << ", "
        << "Armable: " << (health.is_armable ? "YES" : "NO");

    // Enqueue log message instead of writing directly
    enqueue_log(oss.str());
}


void TelemetryLogger::log_all(Telemetry& telemetry) {
    // Subscribe to position updates
    telemetry.subscribe_position_velocity_ned([this](Telemetry::PositionVelocityNed position) {
        log_position(position);
    });

    // Subscribe to battery updates
    telemetry.subscribe_battery([this](Telemetry::Battery battery) {
        log_battery(battery);
    });

    // Subscribe to health updates
    telemetry.subscribe_health([this](Telemetry::Health health) {
        log_health(health);
    });

    // Subscribe to status text messages (if needed)
    telemetry.subscribe_status_text([this](Telemetry::StatusText status) {
        log_status_text(status.text);
    });
}

std::string TelemetryLogger::format_double(double value, int precision) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

std::string TelemetryLogger::current_time() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%H:%M:%S");
    return oss.str();
}
