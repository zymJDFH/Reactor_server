#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port,int subthreadnum,int workthreadnum):tcpserver_(ip,port,subthreadnum),threadpool_(workthreadnum,"WORKS")
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection,this,std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError,this,std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut,this,std::placeholders::_1));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete,this,std::placeholders::_1));
    
}

EchoServer::~EchoServer()
{
}

//启动服务
void EchoServer::Start(){
    tcpserver_.start();
}
void EchoServer::Stop(){
    threadpool_.stop();
    tcpserver_.stop();
    
}
void EchoServer::HandleNewConnection(spConnection conn){
    (void)conn;
}
void EchoServer::HandleClose(spConnection conn){
    (void)conn;
}
void EchoServer::HandleError(spConnection conn){
    (void)conn;
}
void EchoServer::HandleMessage(spConnection conn,std::string &message){
    //printf("EchoServer::HandleMessage() thread is %ld.\n",syscall(SYS_gettid));
    if(threadpool_.size()==0){
        OnMessage(conn,message);
    }
    else{
        //把业务添加到线程池的任务队列中，交给工作线程去处理业务
        threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
    }
}
//处理客户端的请求报文，用于添加给线程池
void EchoServer::OnMessage(spConnection conn,std::string &message){
   // printf("%s message (eventfd=%d):%s\n",Timestamp::now().tostring().c_str(),conn->fd(),message.c_str());
    static const std::string body="ok";
    std::string response;
    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: text/plain\r\n");
    response.append("Content-Length: "+std::to_string(body.size())+"\r\n");
    response.append("Connection: keep-alive\r\n");
    response.append("\r\n");
    response.append(body);
    conn->send(response.data(),response.size());  // 发送 HTTP 响应
}

void EchoServer::HandleSendComplete(spConnection conn){
    //std::cout<<"Message send complete."<<std::endl;
}
void EchoServer::HandleTimeOut(EventLoop *loop){
    //std::cout<<"EchoServer timeout."<<std::endl;
}
