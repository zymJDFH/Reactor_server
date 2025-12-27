#include "TcpServer.h"
TcpServer::TcpServer(const std::string &ip,const uint16_t port,int threadnum)
            :threadnum_(threadnum),mainloop_(new EventLoop(true)),acceptor_(mainloop_.get(),ip,port),threadpool_(threadnum,"IO")
{
    
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));

    //acceptor_ =new Acceptor(mainloop_,ip,port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    
    //threadpool_=new ThreadPool(threadnum,"IO");
    //创建从事件循环
    for(int i=0;i<threadnum_;i++){
        
        subloops_.emplace_back(new EventLoop(false,5,10));
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        subloops_[i]->settimercallback(std::bind(&TcpServer::removeconn,this,std::placeholders::_1));
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
void TcpServer::stop(){
    mainloop_->stop();
    printf("主事件循环已经停止\n");

    //停止从事件循环
    for(int i=0;i<threadnum_;i++){
        subloops_[i]->stop();
    }
    printf("从事件循环停止\n");
    //停止io线程
    threadpool_.stop();
    printf("IO线程池停止\n");

}
void TcpServer::newconnection(std::unique_ptr<Socket>clientsock){
    
    int fd = clientsock->fd();
    int subloop_idx = fd % threadnum_;
    spConnection conn(new Connection(subloops_[subloop_idx].get(),std::move(clientsock)));  //连接负载均衡策略
    
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    //printf("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    {
        std::lock_guard<std::mutex>gd(mmutex_);
        conns_[conn->fd()]=conn;
    }
    subloops_[subloop_idx]->newconnection(conn);
    if(newconnectioncb_)newconnectioncb_(conn);

}
void TcpServer::closeconnection(spConnection conn){
    if(closeconnectioncb_)closeconnectioncb_(conn);
    //printf("client(eventfd=%d) disconnected.\n",conn->fd());
    {
        std::lock_guard<std::mutex>gd(mmutex_);
        conns_.erase(conn->fd());
    }
    
}
void TcpServer::errorconnection(spConnection conn){
    if(errorconnectioncb_)errorconnectioncb_(conn);
    //printf("client(eventfd=%d) error.\n",conn->fd());
    {
        std::lock_guard<std::mutex>gd(mmutex_);
        conns_.erase(conn->fd());
    }
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
void TcpServer::removeconn(int fd){
    {
        std::lock_guard<std::mutex>gd(mmutex_);
        conns_.erase(fd);

    }
}