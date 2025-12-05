#include "Epoll.h"

Epoll::Epoll(){
    if((epollfd_=epoll_create(1))==-1){
        printf("epoll_create() failed(%d).\n",errno);
        exit(-1);
    }
}
Epoll::~Epoll(){
    ::close(epollfd_);
}

std::vector<Channel*> Epoll::loop(int timeout){
    std::vector<Channel*>channels;
    bzero(events_,sizeof events_);
    int nready=epoll_wait(epollfd_,events_,MaxEvents,timeout); //监测epfd中的所有fd
    if(nready<0){
        perror("epoll_wait() failed");
        exit(-1);
    }
    if(nready==0){
        return channels;
    }
    for(int i=0;i<nready;i++){
        Channel *ch=(Channel*)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        channels.push_back(ch);
    }
    return channels;
}

//把channel添加到红黑树上
void Epoll::updatechannel(Channel*ch){
    epoll_event ev;
    ev.data.ptr=ch;
    ev.events=ch->events();
    if(ch->inepoll()){
        if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&ev)==-1){
            perror("epoll_ctl failed()");
            exit(-1);
        }
    }else{
        if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&ev)==-1){
            perror("epoll_ctl failed()");
            exit(-1);
        }
        ch->setinepoll();
    }
}