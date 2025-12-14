#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>
class TcpServer{
private:
    EventLoop *mainloop_; //主事件循环
    std::vector<EventLoop*>subloops_; //从事件循环
    Acceptor *acceptor_;
    ThreadPool *threadpool_;
    int threadnum_;
    std::map<int,Connection*>conns_;//一个TcpServer有多个Connection对象存在map容器中
    std::function<void(Connection*)>newconnectioncb_;
    std::function<void(Connection*)>closeconnectioncb_;
    std::function<void(Connection*)>errorconnectioncb_;
    std::function<void(Connection*,std::string &message)>onmessagecb_;
    std::function<void(Connection*)>sendcompletecb_;
    std::function<void(EventLoop *)>timeoutcb_;
public:
    TcpServer(const std::string &ip,const uint16_t port,int threadnum=3);
    ~TcpServer();
    void start();
    void newconnection(Socket * clientsock);
    void closeconnection(Connection*conn);
    void errorconnection(Connection*conn);
    void onmessage(Connection*conn,std::string &message);
    void sendcomplete(Connection *conn);
    void epolltimeout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(Connection*)>fn);
    void setcloseconnectioncb(std::function<void(Connection*)>fn);
    void seterrorconnectioncb(std::function<void(Connection*)>fn);
    void setonmessagecb(std::function<void(Connection*,std::string &message)>fn);
    void setsendcompletecb(std::function<void(Connection*)>fn);
    void settimeoutcb(std::function<void(EventLoop *)>fn);

};