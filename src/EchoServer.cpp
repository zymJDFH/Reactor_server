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

void EchoServer::HandleNewConnection(spConnection conn){
    std::cout<<"New connetion come in."<<std::endl;
    //printf("EchoServer::HandleNewConnection() thread is %ld.\n",syscall(SYS_gettid));
    //根据业务需求可以添加其他代码
}
void EchoServer::HandleClose(spConnection conn){
    std::cout<<"EchoServer conn close."<<std::endl;
}
void EchoServer::HandleError(spConnection conn){
    std::cout<<"EchoServer conn error."<<std::endl;
}
void EchoServer::HandleMessage(spConnection conn,std::string &message){
    //printf("EchoServer::HandleMessage() thread is %ld.\n",syscall(SYS_gettid));
    
    //把业务添加到线程池的任务队列中
    threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
}
//处理客户端的请求报文，用于添加给线程池
void EchoServer::OnMessage(spConnection conn,std::string &message){
    message ="reply:"+message;  //回显业务
    conn->send(message.data(),message.size());  //发送数据
}

void EchoServer::HandleSendComplete(spConnection conn){
    std::cout<<"Message send complete."<<std::endl;
}
void EchoServer::HandleTimeOut(EventLoop *loop){
    //std::cout<<"EchoServer timeout."<<std::endl;
}
