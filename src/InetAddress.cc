#include <stdlib.h>
#include <string.h>

#include <string>

#include "Logger.h"
#include "InetAddress.h"


using namespace ymuduo;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}
// ip与port构造
InetAddress::InetAddress(const std::string& ip, uint16_t port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr) != 1)
        LOG_FATAL("InetAddress::inet_pton() error!");
    
    addr_.sin_port = htons(port);
}

std::string InetAddress::toIp() const {
    char buf[64] = {0};
    if (::inet_ntop(AF_INET, &addr_.sin_addr,  buf, sizeof(buf)) == nullptr)
        LOG_ERROR("InetAddress::inet_ntop() error!");

    return std::string(buf);
}

uint16_t InetAddress::toPort() const {
    return ::ntohs(addr_.sin_port);
}

std::string InetAddress::toIpPort() const {
    std::string ipPort = toIp();
    ipPort.push_back(':');
    ipPort.append(std::to_string(toPort()));
    return ipPort;
}


