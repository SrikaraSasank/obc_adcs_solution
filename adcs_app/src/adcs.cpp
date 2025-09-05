
#include "adcs.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <algorithm>

using json = nlohmann::json;

AdcsApp::AdcsApp(const AdcsConfig& c)
: cfg(c), tlm(cfg.tlm_host, cfg.tlm_port), fault_rx(cfg.fault_port) {
    st.torque_sat = cfg.torque_sat;
}

void AdcsApp::tick(double dt){
    // --- synthetic plant (for standalone demo if no 42 is hooked) ---
    // attitude error vs reference
    Vec3 e = quatErrorVector(st.q_ref, st.q_body);
    // PD control
    Vec3 w_meas = st.w_body + st.gyro_bias;
    st.torque_cmd = (e * (-cfg.kp)) + (w_meas * (-cfg.kd));

    // apply saturation if requested
    auto sat = [&](double u){
        double s = st.saturate ? st.torque_sat : cfg.torque_sat;
        return std::clamp(u, -s, s);
    };
    st.torque_cmd.x = sat(st.torque_cmd.x);
    st.torque_cmd.y = sat(st.torque_cmd.y);
    st.torque_cmd.z = sat(st.torque_cmd.z);

    // integrate simple rigid-body dynamics (no cross-coupling for demo)
    st.w_body.x += st.torque_cmd.x / cfg.Jw * dt;
    st.w_body.y += st.torque_cmd.y / cfg.Jw * dt;
    st.w_body.z += st.torque_cmd.z / cfg.Jw * dt;

    // integrate quaternion kinematics: qdot = 0.5 * q ⊗ [0, w]
    Quat q = st.q_body;
    Quat wq(0, st.w_body.x, st.w_body.y, st.w_body.z);
    Quat qdot = Quat::multiply(q, wq);
    qdot.w *= 0.5; qdot.x *= 0.5; qdot.y *= 0.5; qdot.z *= 0.5;
    st.q_body = Quat(q.w + qdot.w*dt, q.x + qdot.x*dt, q.y + qdot.y*dt, q.z + qdot.z*dt);
    st.q_body.normalize();

    // drift reference slowly so we see action
    if(step % 50 == 0){
        Vec3 axis{0.0, 0.0, 1.0};
        double dpsi = 0.001; // rad per step
        st.q_ref = Quat::multiply(Quat::fromAxisAngle(axis, dpsi), st.q_ref);
        st.q_ref.normalize();
    }

    step++;
}

void AdcsApp::handleFaults(){
    std::vector<uint8_t> buf;
    int n = fault_rx.recv(buf, 1024, 0);
    if(n <= 0) return;
    std::string msg(buf.begin(), buf.end());
    // Commands: "BIAS gx gy gz", "SAT 1/0", "SATVAL val"
    std::istringstream iss(msg);
    std::string cmd; iss >> cmd;
    if(cmd == "BIAS"){
        double gx, gy, gz; if(iss >> gx >> gy >> gz){
            st.gyro_bias = {gx, gy, gz};
        }
    } else if(cmd == "SAT"){
        int s; if(iss >> s){ st.saturate = (s!=0); }
    } else if(cmd == "SATVAL"){
        double v; if(iss >> v){ st.torque_sat = std::max(0.0, v); }
    }
}

void AdcsApp::sendTelemetry(){
    json j;
    j["step"] = step;
    j["q_ref"] = {st.q_ref.w, st.q_ref.x, st.q_ref.y, st.q_ref.z};
    j["q_body"] = {st.q_body.w, st.q_body.x, st.q_body.y, st.q_body.z};
    j["w_body"] = {st.w_body.x, st.w_body.y, st.w_body.z};
    j["gyro_bias"] = {st.gyro_bias.x, st.gyro_bias.y, st.gyro_bias.z};
    j["torque_cmd"] = {st.torque_cmd.x, st.torque_cmd.y, st.torque_cmd.z};
    j["saturate"] = st.saturate;
    j["torque_sat"] = st.torque_sat;

    auto s = j.dump();
    std::vector<uint8_t> bytes(s.begin(), s.end());
    tlm.send(bytes);
}
