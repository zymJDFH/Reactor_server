#pragma once
#include "InetAddress.h"
// 创建一个非阻塞的socket。
int createnonblocking();
class Socket{
private:
    const int fd_;
    std::string ip_;
    uint16_t port_;
public:
    Socket(int fd);
    ~Socket();
    int fd() const;
    std::string ip()const;
    uint16_t port()const;
    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);
    void setkeepalive(bool on);
    void bind(InetAddress& servaddr);
    void listen(int backlog=128);
    int accept(InetAddress&clientaddr);
    void setipport(const std::string &ip,uint16_t port);

};