
#include "adcs.hpp"
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include <iostream>

int main(){
    AdcsConfig cfg;
    AdcsApp app(cfg);
    const double dt = 0.02; // 50 Hz
    for(;;){
        app.handleFaults();
        app.tick(dt);
        app.sendTelemetry();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    return 0;
}
