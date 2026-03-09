#include "Connection.h"
Connection::Connection(EventLoop *loop,std::unique_ptr<Socket> clientsock)
            :loop_(loop),clientsock_(std::move(clientsock)),clientchannel_(new Channel(loop_,clientsock_->fd())),inputbuffer_(2),outputbuffer_(0),disconnect_(false)
{
    //clientchannel_(new Channel(loop_,clientsock->fd()));
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->useet();
}
Connection::~Connection(){
    //printf("Connection 已析构\n");
}
int Connection::fd() const{
    return clientsock_->fd();
}
std::string Connection::ip()const{
    return clientsock_->ip();
}
uint16_t Connection::port()const{
    return clientsock_->port();
} 

void Connection::closecallback(){
    if(disconnect_.exchange(true)) return;
    spConnection self=shared_from_this();
    clientchannel_->remove();
    loop_->removeconnection(fd());
    if(closecallback_) closecallback_(self);
}
void Connection::errorcallback(){
    if(disconnect_.exchange(true)) return;
    spConnection self=shared_from_this();
    clientchannel_->remove();
    loop_->removeconnection(fd());
    if(errorcallback_) errorcallback_(self);
}

void Connection::setclosecallback(std::function<void(spConnection)>fn){
    closecallback_=fn;
}
void Connection::seterrorcallback(std::function<void(spConnection)>fn){
    errorcallback_=fn;
}
void Connection::setonmessagecallback(std::function<void(spConnection,std::string&)>fn){
    onmessagecallback_=fn;
}
void Connection::connectestablished(){
    clientchannel_->tie(shared_from_this());
    clientchannel_->enablereading();
}
//处理对端发送过来的消息
void Connection::onmessage(){
    char buf[1024];
    while(1){
        bzero(&buf, sizeof(buf));
        ssize_t nread=read(fd(),buf,1024);
        if(nread>0){
            inputbuffer_.append(buf,nread);
        }else if(nread==-1&&errno==EINTR){
            //读取信号时信号中断
            continue;
        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            std::string message;
            while(1){
                if(inputbuffer_.pickmessage(message)==false){
                    break; 
                }
                lasttime_=Timestamp::now(); //更新时间戳
                onmessagecallback_(shared_from_this(),message);
            } 
            break;
        }
        else if(nread==0){
            closecallback();        
            break;
        }
        else{
            errorcallback();
            break;
        }
    }    
}
void Connection::send(const char *data,size_t size){
    if(disconnect_==true){
        return ;
    }
    std::shared_ptr<std::string> message(new std::string(data,size));
    //判断当前线程是否为IO线程
    if(loop_->isinloopthread()){
        //如果当前线程是IO线程，直接调用sendinloop()发送数据
        //printf("send()在事件循环的线程中\n");
        sendinloop(message);
    }
    else{
        //如果当前线程不是IO线程，调用EventLoop::queueinloop(),把sendinloop()交给事件循环线程去执行
        //printf("send()不在事件循环的线程中\n");
        spConnection self=shared_from_this();
        loop_->queueinloop([self,message](){
            self->sendinloop(message);
        });
    }
    
}
void Connection::sendinloop(std::shared_ptr<std::string>data){
    if(disconnect_==true) return;
    outputbuffer_.appendwithsep(data->data(),data->size());
    clientchannel_->enablewriting();
}
void Connection::writecallback(){
    int writen=::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);
    if(writen>0){
        outputbuffer_.erase(0,writen);//删除已发送字节数
    }else if((writen==-1) && (errno!=EAGAIN) && (errno!=EWOULDBLOCK) && (errno!=EINTR)){
        errorcallback();
        return;
    }
    if(outputbuffer_.size()==0){
        clientchannel_->disablewriting();
        if(sendcompletecallback_) sendcompletecallback_(shared_from_this());
    }
}

void Connection::setsendcompletecallback(std::function<void(spConnection)>fn){
    sendcompletecallback_=fn;
}
bool Connection::timeout(time_t now,int val){
    return now-lasttime_.toint()>val;
}
