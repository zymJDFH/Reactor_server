#include "Socket.h"
int createNonBlocking(){
    int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenfd<0){
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenfd;
}
Socket::Socket(int fd):fd_(fd){}

Socket::~Socket(){
    ::close(fd_);
}
int Socket::fd() const{
    return fd_;
}
void Socket::setReuseAddr(bool on){
    int opt=on?1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::setReusePort(bool on){
    int opt=on?1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::setTcpNoDelay(bool on){
    int opt=on?1:0;
    ::setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::setKeepAlive(bool on){
    int opt=on?1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::listen(int backlog){
    if(::listen(fd_,backlog)!=0){
        perror("listen() failed"); 
        close(fd_); 
        exit(-1);
    }
}
void Socket::bind(InetAddress& servaddr){
    if(::bind(fd_,servaddr.addr(),sizeof(sockaddr))<0){
        perror("bind failed()");
        close(fd_);
        exit(-1);
    }
}
int Socket::accept(InetAddress&clientaddr){
    sockaddr_in peeraddr;
    socklen_t len= sizeof(peeraddr);
    int clientfd=accept4(fd_,(sockaddr*)&peeraddr,&len,SOCK_NONBLOCK);
    clientaddr.setAddr(peeraddr);
    return clientfd;
}