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
    std::unique_ptr<EventLoop>mainloop_; //主事件循环
    std::vector<std::unique_ptr<EventLoop>>subloops_; //从事件循环
    Acceptor acceptor_;
    int threadnum_;
    ThreadPool threadpool_;
    std::map<int,spConnection>conns_;//一个TcpServer有多个Connection对象存在map容器中
    std::function<void(spConnection)>newconnectioncb_;
    std::function<void(spConnection)>closeconnectioncb_;
    std::function<void(spConnection)>errorconnectioncb_;
    std::function<void(spConnection,std::string &message)>onmessagecb_;
    std::function<void(spConnection)>sendcompletecb_;
    std::function<void(EventLoop *)>timeoutcb_;
public:
    TcpServer(const std::string &ip,const uint16_t port,int threadnum=5);
    ~TcpServer();
    void start();
    void newconnection(std::unique_ptr<Socket> clientsock);
    void closeconnection(spConnection conn);
    void errorconnection(spConnection conn);
    void onmessage(spConnection conn,std::string &message);
    void sendcomplete(spConnection conn);
    void epolltimeout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(spConnection)>fn);
    void setcloseconnectioncb(std::function<void(spConnection)>fn);
    void seterrorconnectioncb(std::function<void(spConnection)>fn);
    void setonmessagecb(std::function<void(spConnection,std::string &message)>fn);
    void setsendcompletecb(std::function<void(spConnection)>fn);
    void settimeoutcb(std::function<void(EventLoop *)>fn);

};