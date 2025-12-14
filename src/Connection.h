#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "memory"
class Connection;
using spConnection=std::shared_ptr<Connection>;
class Connection :public std::enable_shared_from_this<Connection>
{
    
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientchannel_;
    std::function<void(spConnection)>closecallback_;  
    std::function<void(spConnection)>errorcallback_; 
    std::function<void(spConnection,std::string&)>onmessagecallback_; 
    std::function<void(spConnection)>sendcompletecallback_; 
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
    void setclosecallback(std::function<void(spConnection)>fn);
    void seterrorcallback(std::function<void(spConnection)>fn);
    void setonmessagecallback(std::function<void(spConnection,std::string&)>fn);
    void setsendcompletecallback(std::function<void(spConnection)>fn);

    void onmessage();
    void send(const char *data,size_t size);
   
};

