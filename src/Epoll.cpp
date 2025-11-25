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

void Epoll::addfd(int fd,uint32_t op){
    epoll_event ev;
    ev.events=op;
    ev.data.fd=fd;
    
    if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&ev)==-1){
        printf("epoll_ctl() failed(%d).\n",errno); 
        exit(-1);
    }
    
}
std::vector<epoll_event> Epoll::loop(int timeout){
    std::vector<epoll_event>evs;
    bzero(events_,sizeof events_);
    int nready=epoll_wait(epollfd_,events_,MaxEvents,timeout); //监测epfd中的所有fd
    if(nready<0){
        perror("epoll_wait() failed");
        exit(-1);
    }
    if(nready==0){
        std::cout<<"epoll_wait() timeout."<<std::endl;
        return evs;
    }
    for(int i=0;i<nready;i++){
        evs.push_back(events_[i]);
    }
    return evs;
}
int Epoll::epollfd(){
    return epollfd_;
}