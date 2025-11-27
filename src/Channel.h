#pragma once
#include "Epoll.h"
#include "sys/epoll.h"
class Epoll;
class Channel{
private:
    int fd_=-1;
    Epoll *ep_=nullptr;      //channel对应的fd
    bool inepoll_=false;    //channel是否已经添加到epoll树上
    uint32_t events_=0;  //fd_需要监视的事件
    uint32_t revents_=0; //fd_已发生的事件

public:
    Channel(Epoll *ep,int fd);
    ~Channel();

    int fd();
    bool inepoll();
    uint32_t events();
    uint32_t revents();
    void useet();

    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
   
};