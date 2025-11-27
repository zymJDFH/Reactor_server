#include "Channel.h"
Channel::Channel(Epoll *ep,int fd):ep_(ep),fd_(fd){

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
    ep_->updatechannel(this);
}
void Channel::setinepoll(){
    inepoll_=true;
}
void Channel::setrevents(uint32_t ev){
    revents_=ev;
}