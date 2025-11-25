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
    //创建epoll句柄
    int epfd=epoll_create(1);

    epoll_event ev;
    ev.events=EPOLLIN;//listenfd水平触发（持续触发）
    ev.data.fd=servsock.fd();

    epoll_ctl(epfd,EPOLL_CTL_ADD,servsock.fd(),&ev);
    //存放epoll_wait返回事件的数组
    struct epoll_event evs[1024];
    while(1){
        int nready=epoll_wait(epfd,evs,1024,-1); //监测epfd中的所有fd
        if(nready<0){
            perror("epoll_wait() failed");
            break;
        }
        if(nready==0){
           std::cout<<"epoll_wait() timeout."<<std::endl;
           continue;
        }
        
        for(int i=0;i<nready;i++){
            if(evs[i].events&EPOLLRDHUP){//异常断开场景 半关闭处理
                printf("client(eventfd=%d) disconnected.\n",evs[i].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
                close(evs[i].data.fd);
            }else if(evs[i].events&(EPOLLIN|EPOLLPRI)){
                if(evs[i].data.fd==servsock.fd()){
                    InetAddress clientaddr;
                    Socket *clientsock=new Socket(servsock.accept(clientaddr));//堆上
                    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());

                    ev.events=EPOLLIN|EPOLLET|EPOLLRDHUP;//减少事件触发次数 ，降低cpu开销
                    ev.data.fd=clientsock->fd();
                    epoll_ctl(epfd,EPOLL_CTL_ADD,clientsock->fd(),&ev);

                }else{
                    char buf[1024];
                    while(1){
                        bzero(&buf, sizeof(buf));
                        ssize_t nread=read(evs[i].data.fd,buf,1024);
                        if(nread>0){
                            printf("recv(eventfd=%d):%s\n",evs[i].data.fd,buf);
                            send(evs[i].data.fd,buf,nread,0);
                        }else if(nread==-1&&errno==EINTR){
                            //读取信号时信号中断
                            continue;
                        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                            break;
                        }
                        else if(nread==0){
                            printf("client(eventfd=%d) disconnected.\n",evs[i].data.fd);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
                            close(evs[i].data.fd);           
                            break;
                        }
                    }
                    
                }
            }
            else if(evs[i].events&EPOLLOUT){

            }else{//其他事件都为错误
                printf("client(eventfd=%d) error.\n",evs[i].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
                close(evs[i].data.fd); 
            }
                
        }
    }
    return 0;
}
