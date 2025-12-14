#include "EventLoop.h"
EventLoop::EventLoop():ep_(new Epoll){
    
}
EventLoop::~EventLoop(){
    //delete ep_;
}
#include <unistd.h>
#include <sys/syscall.h>
void EventLoop::run(){
    //printf("EventLoop::run() thread is %ld\n",syscall(SYS_gettid));
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
void EventLoop::removechannel(Channel*ch){
    ep_->removechannel(ch);
}

//设置epollwait超时的回调函数
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)>fn){
    epolltimeoutcallback_=fn;
}