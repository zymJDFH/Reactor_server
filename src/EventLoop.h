#pragma once
#include "Epoll.h"
#include "Channel.h"
class Channel;
class Epoll;
class EventLoop{
private:
    Epoll *ep_;
public:
    EventLoop();
    ~EventLoop();
    void run();
   
    void updatechannel(Channel*ch);
};