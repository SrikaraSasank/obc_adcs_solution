
#pragma once
#include <cstdint>
#include <string>
#include <vector>

class UdpSender {
    int fd{-1};
    struct Impl;
    Impl* p;
public:
    UdpSender(const std::string& host, uint16_t port);
    ~UdpSender();
    bool send(const std::vector<uint8_t>& bytes);
};

class UdpReceiver {
    int fd{-1};
    struct Impl;
    Impl* p;
public:
    UdpReceiver(uint16_t port);
    ~UdpReceiver();
    // returns number of bytes, 0 if no data, -1 on fatal error
    int recv(std::vector<uint8_t>& out, int max_bytes=2048, int timeout_ms=0);
};
