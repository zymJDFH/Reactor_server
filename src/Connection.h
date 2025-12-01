#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
class Connection
{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientchannel_;
public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();
};

