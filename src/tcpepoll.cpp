#include<sys/socket.h>
#include<iostream>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<arpa/inet.h>  
#include<unistd.h>
#include<cstring>
#include <netinet/tcp.h>  
#include"InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
int main(int argc, char const *argv[])
{
     if(argc!=3){
        std::cout<<"ip port"<<std::endl;
        return -1;
    }
    InetAddress servaddr(argv[1],atoi(argv[2]));
    Socket servsock(createNonBlocking());
    servsock.setKeepAlive(true);
    servsock.setReuseAddr(true);
    servsock.setReusePort(true);
    servsock.setTcpNoDelay(true);
    servsock.bind(servaddr);
    servsock.listen();
    
    Epoll ep;
    ep.addfd(servsock.fd(),EPOLLIN);
    //存放epoll_wait返回事件
    std::vector<epoll_event>evs;
 
    while(1){
        evs=ep.loop();

        for(auto &ev:evs){
            if(ev.events&EPOLLRDHUP){//异常断开场景 半关闭处理
                printf("client(eventfd=%d) disconnected.\n",ev.data.fd);
                epoll_ctl(ep.epollfd(), EPOLL_CTL_DEL, ev.data.fd, NULL);
                close(ev.data.fd);
            }else if(ev.events&(EPOLLIN|EPOLLPRI)){
                if(ev.data.fd==servsock.fd()){
                    InetAddress clientaddr;
                    Socket *clientsock=new Socket(servsock.accept(clientaddr));//堆上
                    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());
                    ep.addfd(clientsock->fd(),EPOLLIN|EPOLLET|EPOLLRDHUP);//
                }else{
                    char buf[1024];
                    while(1){
                        bzero(&buf, sizeof(buf));
                        ssize_t nread=read(ev.data.fd,buf,1024);
                        if(nread>0){
                            printf("recv(eventfd=%d):%s\n",ev.data.fd,buf);
                            send(ev.data.fd,buf,nread,0);
                        }else if(nread==-1&&errno==EINTR){
                            //读取信号时信号中断
                            continue;
                        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                            break;
                        }
                        else if(nread==0){
                            printf("client(eventfd=%d) disconnected.\n",ev.data.fd);
                            epoll_ctl(ep.epollfd(), EPOLL_CTL_DEL, ev.data.fd, NULL);
                            close(ev.data.fd);           
                            break;
                        }
                    }
                    
                }
            }
            else if(ev.events&EPOLLOUT){

            }else{//其他事件都为错误
                printf("client(eventfd=%d) error.\n",ev.data.fd);
                epoll_ctl(ep.epollfd(), EPOLL_CTL_DEL, ev.data.fd, NULL);
                close(ev.data.fd); 
            }
                
        }
    }
    return 0;
}
