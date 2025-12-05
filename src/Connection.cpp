#include "Connection.h"
Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock){
    clientchannel_ =new Channel(loop_,clientsock->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));//?
    //clientchannel_->useet();
    clientchannel_->enablereading();
}
Connection::~Connection(){
    delete clientchannel_;
    delete clientsock_;
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
    // printf("client(eventfd=%d) disconnected.\n",fd());
    // close(fd());
    closecallback_(this);
}
void Connection::errorcallback(){
    errorcallback_(this);
}

void Connection::setclosecallback(std::function<void(Connection*)>fn){
    closecallback_=fn;
}
void Connection::seterrorcallback(std::function<void(Connection*)>fn){
    errorcallback_=fn;
}
void Connection::setonmessagecallback(std::function<void(Connection*,std::string)>fn){
    onmessagecallback_=fn;
}
//处理对端发送过来的消息
void Connection::onmessage(){
    char buf[1024];
    while(1){
        bzero(&buf, sizeof(buf));
        ssize_t nread=read(fd(),buf,1024);
        if(nread>0){
            // printf("recv(eventfd=%d):%s\n",fd(),buf);
            // send(fd(),buf,nread,0);
            inputbuffer_.append(buf,nread);
        }else if(nread==-1&&errno==EINTR){
            //读取信号时信号中断
            continue;
        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){

           
            while(1){
                int len;
                memcpy(&len,inputbuffer_.data(),4);//获取报文头部
                if(inputbuffer_.size()<len+4){
                    //报文不完整
                    break;
                }
                std::string message(inputbuffer_.data()+4,len);
                inputbuffer_.erase(0,len+4);

                printf("message (eventfd=%d):%s\n",fd(),message.c_str());
                // message ="reply:"+message;
                // len=message.size();
                // std::string tmpbuf((char*)&len,4);
                // tmpbuf.append(message);
                // send(fd(),tmpbuf.data(),tmpbuf.size(),0);
                onmessagecallback_(this,message);
            } 
           
            break;
        }
        else if(nread==0){
            closecallback();        
            break;
        }
    }    
}
void Connection::send(const char *data,size_t size){
    outputbuffer_.append(data,size);
    clientchannel_->enablewriting();
}
void Connection::writecallback(){
    int writen=::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);
    if(writen>0){
        outputbuffer_.erase(0,writen);//删除已发送字节数
    }
    if(outputbuffer_.size()==0){
        clientchannel_->disablewriting();
        sendcompletecallback_(this);
    }
}

void Connection::setsendcompletecallback(std::function<void(Connection*)>fn){
    sendcompletecallback_=fn;
}

