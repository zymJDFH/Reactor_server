#include "Channel.h"

Channel::Channel(Epoll *ep,int fd,bool islisten):ep_(ep),fd_(fd),islisten_(islisten){

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
void Channel::haneleevent(Socket *servsock){
    if(revents_&EPOLLRDHUP){//异常断开场景 半关闭处理
        printf("client(eventfd=%d) disconnected.\n",fd_);
        close(fd_);
    }else if(revents_&(EPOLLIN|EPOLLPRI)){
        if(islisten_){
            InetAddress clientaddr;
            Socket *clientsock=new Socket(servsock->accept(clientaddr));//堆上
            printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());
            
            Channel *clientchannel =new Channel(ep_,clientsock->fd(),false);
            clientchannel->useet();
            clientchannel->enablereading();

        }else{
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
                    printf("client(eventfd=%d) disconnected.\n",fd_);
                    close(fd_);           
                    break;
                }
            }    
        }
    }
    else if(revents_&EPOLLOUT){

    }else{//其他事件都为错误
        printf("client(eventfd=%d) error.\n",fd_);
        close(fd_); 
    }
                
}