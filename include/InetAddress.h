#ifndef YMUDUO_INETADDRESS
#define YMUDUO_INETADDRESS

#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>


// 封装socket地址类型

namespace ymuduo {

class InetAddress {
public:
    //本地环回
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
    // ip与port构造
    InetAddress(const std::string& ip, uint16_t port);
    //sockaddr构造
    explicit InetAddress(const struct sockaddr_in& addr) : addr_(addr) {}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const{ return reinterpret_cast<const struct sockaddr*>(&addr_); }

    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; };


private:
    struct sockaddr_in addr_;
};



}

#endif