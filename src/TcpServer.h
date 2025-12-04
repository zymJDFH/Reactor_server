#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>
class TcpServer{
private:
    EventLoop loop_; //单线程一个事件循环
    Acceptor *acceptor_;
    std::map<int,Connection*>conns_;//一个TcpServer有多个Connection对象存在map容器中
public:
    TcpServer(const std::string &ip,const uint16_t port);
    ~TcpServer();
    void start();
    void newconnection(Socket *clientsock);
    void closeconnection(Connection*conn);
    void errorconnection(Connection*conn);
    void onmessage(Connection*conn,std::string message);
};