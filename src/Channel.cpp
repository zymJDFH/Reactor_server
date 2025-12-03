#include "Channel.h"

Channel::Channel(EventLoop *loop,int fd):loop_(loop),fd_(fd){

}
Channel::~Channel(){

}

int Channel::fd(){
    return fd_;
}
bool Channel::inepoll(){
    return inepoll_;
}
uint32_t Channel::events(){
    return events_;
}
uint32_t Channel::revents(){
    return revents_;
}
void Channel::useet(){
    events_|=EPOLLET;
}

void Channel::enablereading(){
    events_|=EPOLLIN;
    loop_->updatechannel(this);
}
void Channel::setinepoll(){
    inepoll_=true;
}
void Channel::setrevents(uint32_t ev){
    revents_=ev;
}
void Channel::handleevent(){
    if(revents_&EPOLLRDHUP){//异常断开场景 半关闭处理
        closecallback_(); 
    }else if(revents_&(EPOLLIN|EPOLLPRI)){
        readcallback_();
    }
    else if(revents_&EPOLLOUT){

    }else{//其他事件都为错误
        errorcallback_(); 
    }
                
}
//处理对端发送过来的消息
void Channel::onmessage(){
    char buf[1024];
    while(1){
        bzero(&buf, sizeof(buf));
        ssize_t nread=read(fd_,buf,1024);
        if(nread>0){
            printf("recv(eventfd=%d):%s\n",fd_,buf);
            send(fd_,buf,nread,0);
        }else if(nread==-1&&errno==EINTR){
            //读取信号时信号中断
            continue;
        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            break;
        }
        else if(nread==0){
            closecallback_();        
            break;
        }
    }    
}

void Channel::setreadcallback(std::function<void()>fn){
    readcallback_=fn;
}
void Channel::seterrorcallback(std::function<void()>fn){
    errorcallback_=fn;
}
void Channel::setclosecallback(std::function<void()>fn){
    closecallback_=fn;
}