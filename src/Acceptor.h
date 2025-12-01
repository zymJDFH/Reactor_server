#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
class Acceptor
{
private:
    EventLoop *loop_;
    Socket *servsock_;
    Channel *acceptchannel_;
    std::function<void(Socket*)>newconnectioncb_;
public:
    Acceptor(EventLoop *loop,const std::string &ip,const uint16_t port);
    ~Acceptor();
    void newconnection();
    void setnewconnectioncb(std::function<void(Socket*)>fn);
};

