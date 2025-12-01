#pragma once
#include "EventLoop.h"
#include "sys/epoll.h"
#include "Socket.h"
#include <functional>

class Epoll;
class EventLoop;
class Channel{//事件管理
private:
    int fd_=-1;
    EventLoop *loop_=nullptr;      //channel对应的fd
    bool inepoll_=false;    //channel是否已经添加到epoll树上
    uint32_t events_=0;  //fd_需要监视的事件
    uint32_t revents_=0; //fd_已发生的事件
   // bool islisten_=false;
    std::function<void()>readcallback_; //fd读事件的回调函数

public:
    Channel(EventLoop *loop,int fd);
    ~Channel();

    int fd();
    bool inepoll();
    uint32_t events();
    uint32_t revents();
    void useet();

    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
    void handleevent();
    void newconnection(Socket *servsock);
    void onmessage();
    void setreadcallback(std::function<void()>fn);
};