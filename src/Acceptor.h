#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <memory>
class Acceptor
{
private:
    const std::unique_ptr<EventLoop>&loop_;
    Socket servsock_;
    Channel acceptchannel_;
    std::function<void(std::unique_ptr<Socket>)>newconnectioncb_;
public:
    Acceptor(const std::unique_ptr<EventLoop>&loop,const std::string &ip,const uint16_t port);
    ~Acceptor();
    void newconnection();
    void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)>fn);
};

