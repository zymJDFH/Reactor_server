#include "TcpServer.h"
TcpServer::TcpServer(const std::string &ip,const uint16_t port,int threadnum)
            :threadnum_(threadnum),mainloop_(new EventLoop),acceptor_(mainloop_,ip,port),threadpool_(threadnum,"IO")
{
    
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));

    //acceptor_ =new Acceptor(mainloop_,ip,port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    
    //threadpool_=new ThreadPool(threadnum,"IO");
    //创建从事件循环
    for(int i=0;i<threadnum_;i++){
        
        subloops_.emplace_back(new EventLoop);
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run,subloops_[i].get()));//在线程池中运行从事件循环
    }
}
TcpServer::~TcpServer(){
    //delete acceptor_;
    //delete mainloop_;
    //释放全部Connection对象
    // for(auto &it:conns_){
    //     delete it.second;
    // }
    //delete threadpool_;
}

void TcpServer::start(){
    mainloop_->run();
}
void TcpServer::newconnection(std::unique_ptr<Socket> clientsock){
    //Connection *conn =new Connection(mainloop_,clientsock);
    spConnection conn(new Connection(subloops_[clientsock->fd()%threadnum_],std::move(clientsock)));  //连接负载均衡策略
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    //printf("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    conns_[conn->fd()]=conn;
    if(newconnectioncb_)newconnectioncb_(conn);
}
void TcpServer::closeconnection(spConnection conn){
    if(closeconnectioncb_)closeconnectioncb_(conn);
    //printf("client(eventfd=%d) disconnected.\n",conn->fd());
    
    conns_.erase(conn->fd());
    //delete conn;
}
void TcpServer::errorconnection(spConnection conn){
    if(errorconnectioncb_)errorconnectioncb_(conn);
    //printf("client(eventfd=%d) error.\n",conn->fd());
   
    conns_.erase(conn->fd());
    //delete conn;
}
void TcpServer::onmessage(spConnection conn,std::string &message){
   if(onmessagecb_)onmessagecb_(conn,message);
}

//数据发送完成后，在Connection中回调此函数
void TcpServer::sendcomplete(spConnection conn){
   
    if(sendcompletecb_)sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop *loop){
  
    if(timeoutcb_)timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)>fn){
    newconnectioncb_=fn;
}
void TcpServer::setcloseconnectioncb(std::function<void(spConnection)>fn){
    closeconnectioncb_=fn;
}
void TcpServer::seterrorconnectioncb(std::function<void(spConnection)>fn){
    errorconnectioncb_=fn;
}
void TcpServer::setonmessagecb(std::function<void(spConnection,std::string &message)>fn){
    onmessagecb_=fn;
}
void TcpServer::setsendcompletecb(std::function<void(spConnection)>fn){
    sendcompletecb_=fn;
}
void TcpServer::settimeoutcb(std::function<void(EventLoop *)>fn){
    timeoutcb_=fn;
}