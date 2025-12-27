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
void Channel::disablereading(){
    events_&=~EPOLLIN;
    loop_->updatechannel(this);
}
void Channel::enablewriting(){
    events_|=EPOLLOUT;
    loop_->updatechannel(this);
}  
void Channel::disablewriting(){
    events_&=~EPOLLOUT;
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
        writecallback_();
    }else{//其他事件都为错误
        errorcallback_(); 
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
void Channel::setwritecallback(std::function<void()>fn){
    writecallback_=fn;
}

void Channel::disableall(){
    //取消全部事件
    events_=0;
    loop_->updatechannel(this);
    
}     
void Channel::remove(){
    //从事件循环中删除Channel  
    disableall();
    loop_->removechannel(this);//从红黑树上删除fd
}              