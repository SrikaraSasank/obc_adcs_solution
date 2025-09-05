
#include "udp_link.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <chrono>

struct UdpSender::Impl {
    sockaddr_in addr{};
};

UdpSender::UdpSender(const std::string& host, uint16_t port) : p(new Impl){
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&p->addr, 0, sizeof(p->addr));
    p->addr.sin_family = AF_INET;
    p->addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &p->addr.sin_addr);
}

UdpSender::~UdpSender(){
    if(fd>=0) close(fd);
    delete p;
}

bool UdpSender::send(const std::vector<uint8_t>& bytes){
    if(fd<0) return false;
    ssize_t n = ::sendto(fd, bytes.data(), bytes.size(), 0, (sockaddr*)&p->addr, sizeof(p->addr));
    return n == (ssize_t)bytes.size();
}

struct UdpReceiver::Impl {
    sockaddr_in addr{};
};

UdpReceiver::UdpReceiver(uint16_t port) : p(new Impl){
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    int enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    memset(&p->addr, 0, sizeof(p->addr));
    p->addr.sin_family = AF_INET;
    p->addr.sin_addr.s_addr = INADDR_ANY;
    p->addr.sin_port = htons(port);
    bind(fd, (sockaddr*)&p->addr, sizeof(p->addr));
    // non-blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

UdpReceiver::~UdpReceiver(){
    if(fd>=0) close(fd);
    delete p;
}

int UdpReceiver::recv(std::vector<uint8_t>& out, int max_bytes, int timeout_ms){
    out.resize(max_bytes);
    int total_wait = 0;
    for(;;){
        ssize_t n = ::recvfrom(fd, out.data(), max_bytes, 0, nullptr, nullptr);
        if(n >= 0){
            out.resize((size_t)n);
            return (int)n;
        }
        if(timeout_ms <= 0) return 0;
        if(total_wait >= timeout_ms) return 0;
        usleep(1000);
        total_wait += 1;
    }
}
