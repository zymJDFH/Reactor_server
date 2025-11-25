#pragma once
#include "InetAddress.h"
// 创建一个非阻塞的socket。
int createNonBlocking();
class Socket{
private:
    const int fd_;
public:
    Socket(int fd);
    ~Socket();
    int fd() const;
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setTcpNoDelay(bool on);
    void setKeepAlive(bool on);
    void bind(InetAddress& servaddr);
    void listen(int backlog=128);
    int accept(InetAddress&clientaddr);
};