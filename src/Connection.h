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
    std::function<void(Connection*)>closecallback_;  
    std::function<void(Connection*)>errorcallback_; 
     
    
public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();
    int fd() const;
    std::string ip()const;
    uint16_t port()const;

    void closecallback();
    void errorcallback();

    void setclosecallback(std::function<void(Connection*)>fn);
    void seterrorcallback(std::function<void(Connection*)>fn);
};

