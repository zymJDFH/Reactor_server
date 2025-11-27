#pragma once
#include "InetAddress.h"
// 创建一个非阻塞的socket。
int createnonblocking();
class Socket{
private:
    const int fd_;
public:
    Socket(int fd);
    ~Socket();
    int fd() const;
    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);
    void setkeepalive(bool on);
    void bind(InetAddress& servaddr);
    void listen(int backlog=128);
    int accept(InetAddress&clientaddr);
};