#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
class Connection
{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientchannel_;
    std::function<void(Connection*)>closecallback_;  
    std::function<void(Connection*)>errorcallback_; 
    std::function<void(Connection*,std::string)>onmessagecallback_; 
    std::function<void(Connection*)>sendcompletecallback_; 
    Buffer inputbuffer_;
    Buffer outputbuffer_;
public: 
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();
    int fd() const;
    std::string ip()const;
    uint16_t port()const;

    void closecallback();
    void errorcallback();
    void writecallback();
    void setclosecallback(std::function<void(Connection*)>fn);
    void seterrorcallback(std::function<void(Connection*)>fn);
    void setonmessagecallback(std::function<void(Connection*,std::string)>fn);
    void setsendcompletecallback(std::function<void(Connection*)>fn);

    void onmessage();
    void send(const char *data,size_t size);
   
};

