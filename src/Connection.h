#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include <memory>
#include <atomic>
#include "Timestamp.h"
class EventLoop;
class Connection;
class Channel;
using spConnection=std::shared_ptr<Connection>;
class Connection :public std::enable_shared_from_this<Connection>
{
    
private:
    EventLoop *loop_;
    std::unique_ptr<Socket>clientsock_;
    std::unique_ptr<Channel>clientchannel_;
    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic_bool disconnect_;   //客户端连接是否断开，如果已经断开，设置为true
    std::function<void(spConnection)>closecallback_;  
    std::function<void(spConnection)>errorcallback_; 
    std::function<void(spConnection,std::string&)>onmessagecallback_; 
    std::function<void(spConnection)>sendcompletecallback_; 
    Timestamp lasttime_;    //创建Connection对象时为当前时间，每接受到一个报文，把时间戳更新为当前时间

public: 
    Connection(EventLoop *loop,std::unique_ptr<Socket> clientsock);
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
    //void sendinloop(const char *data,size_t size);
    void sendinloop(std::shared_ptr<std::string>data);
    bool timeout(time_t now,int val);
};

