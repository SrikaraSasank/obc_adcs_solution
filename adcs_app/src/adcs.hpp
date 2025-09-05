
#pragma once
#include "math_quat.hpp"
#include "udp_link.hpp"
#include <cstdint>
#include <vector>
#include <string>

struct AdcsConfig {
    // Control gains
    double kp{0.12};
    double kd{0.035};
    // Wheel inertia (simple single-axis equivalent for demo)
    double Jw{0.002};
    // Torque saturation (Nm)
    double torque_sat{0.05};
    // Telemetry ports
    std::string tlm_host{"127.0.0.1"};
    uint16_t tlm_port{5005};
    uint16_t fault_port{5006};
};

struct AdcsState {
    Quat q_ref{1,0,0,0};
    Quat q_body{1,0,0,0};
    Vec3 w_body{0,0,0};        // rad/s
    Vec3 gyro_bias{0,0,0};     // injected bias
    Vec3 torque_cmd{0,0,0};    // Nm
    bool saturate{false};
    double torque_sat{0.05};
};

// Minimal binary framing for simulator I/O (placeholder)
// This demo generates synthetic body rates and drifts if no simulator is present.
class AdcsApp {
    AdcsConfig cfg;
    AdcsState st;
    UdpSender tlm;
    UdpReceiver fault_rx;
    uint64_t step{0};
public:
    explicit AdcsApp(const AdcsConfig& c);
    void tick(double dt); // run one control step
    void handleFaults();  // listen to UDP fault commands
    void sendTelemetry(); // JSON over UDP
};
