#pragma once
#include <sys/epoll.h>
#include <vector>
#include<iostream>
#include<unistd.h>
#include <errno.h>
#include<string.h>
#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
class Channel;

class Epoll{
private:
    static const int MaxEvents=100;
    int epollfd_=-1;//epoll句柄
    epoll_event events_[MaxEvents]; //存放epoll_wait返回事件的数组
public:
    Epoll();
    ~Epoll();
    void updatechannel(Channel*ch);
    void removechannel(Channel*ch);
    
    std::vector<Channel*>loop(int timeout=-1);
    
};
