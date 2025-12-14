#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <functional>
#include <memory>

class Channel;
class Epoll;
class EventLoop{
private:
    std::unique_ptr<Epoll>ep_;
    std::function<void(EventLoop*)>epolltimeoutcallback_;
public:
    EventLoop();
    ~EventLoop();
    void run();
   
    void updatechannel(Channel*ch);
    void removechannel(Channel*ch);
    void setepolltimeoutcallback(std::function<void(EventLoop*)>fn);
};