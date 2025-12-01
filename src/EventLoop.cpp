#include "EventLoop.h"
EventLoop::EventLoop(){
    ep_=new Epoll;
}
EventLoop::~EventLoop(){
    delete ep_;
}

void EventLoop::run(){
    while(1){
        std::vector<Channel*>channels=ep_->loop();
        for(auto &ch:channels){
            ch->handleevent();
        }
    }
}

void EventLoop::updatechannel(Channel*ch){
    ep_->updatechannel(ch);
}