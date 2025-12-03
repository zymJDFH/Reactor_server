#include "TcpServer.h"
TcpServer::TcpServer(const std::string &ip,const uint16_t port){
    acceptor_ =new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
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