#include "TcpServer.h"
TcpServer::TcpServer(const std::string &ip,const uint16_t port){
    acceptor_ =new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
}
TcpServer::~TcpServer(){
    delete acceptor_;
    //释放全部Connection对象
    for(auto &it:conns_){
        delete it.second;
    }
}

void TcpServer::start(){
    loop_.run();
}
void TcpServer::newconnection(Socket *clientsock){
    Connection *conn =new Connection(&loop_,clientsock);
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    printf("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    conns_[conn->fd()]=conn;
}
void TcpServer::closeconnection(Connection*conn){
    printf("client(eventfd=%d) disconnected.\n",conn->fd());
    //close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}
void TcpServer::errorconnection(Connection*conn){
    printf("client(eventfd=%d) error.\n",conn->fd());
    //close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}
void TcpServer::onmessage(Connection*conn,std::string message){
    message ="reply:"+message;
    int len=message.size();
    std::string tmpbuf((char*)&len,4);
    tmpbuf.append(message);
    conn->send(tmpbuf.data(),tmpbuf.size());
}

//数据发送完成后，在Connection中回调此函数
void TcpServer::sendcomplete(Connection *conn){
    printf("send complete\n");
    //业务
}

void TcpServer::epolltimeout(EventLoop *loop){
    printf("epoll_wait() timeout\n");
    //业务
}