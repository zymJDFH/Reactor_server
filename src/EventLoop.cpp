#include "EventLoop.h"
EventLoop::EventLoop(){
    ep_=new Epoll;
}
EventLoop::~EventLoop(){
    delete ep_;
}

void EventLoop::run(){
    while(1){
        std::vector<Channel*>channels=ep_->loop(10*1000);
        if(channels.size()==0){
            epolltimeoutcallback_(this);
        }else{
            for(auto &ch:channels){
                ch->handleevent();
            }
        }
        
    }
}

void EventLoop::updatechannel(Channel*ch){
    ep_->updatechannel(ch);
}
//设置epollwait超时的回调函数
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)>fn){
    epolltimeoutcallback_=fn;
}