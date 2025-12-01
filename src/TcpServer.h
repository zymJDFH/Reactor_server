#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Acceptor.h"
class TcpServer{
private:
    EventLoop loop_; //单线程一个事件循环
    Acceptor *acceptor_;
public:
    TcpServer(const std::string &ip,const uint16_t port);
    ~TcpServer();
    void start();
};