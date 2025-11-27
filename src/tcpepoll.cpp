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
    Socket servsock(createnonblocking());
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);
    servsock.bind(servaddr);
    servsock.listen();
    
    Epoll ep;
    //ep.addfd(servsock.fd(),EPOLLIN);
    Channel *servchannel =new Channel(&ep,servsock.fd());
    servchannel->enablereading();
    //存放epoll_wait返回事件
    //std::vector<epoll_event>evs;
 
    while(1){
        std::vector<Channel*>channels=ep.loop();

        for(auto &ch:channels){
            if(ch->revents()&EPOLLRDHUP){//异常断开场景 半关闭处理
                printf("client(eventfd=%d) disconnected.\n",ch->fd());
                epoll_ctl(ep.epollfd(), EPOLL_CTL_DEL, ch->fd(), NULL);
                close(ch->fd());
            }else if(ch->revents()&(EPOLLIN|EPOLLPRI)){
                if(ch==servchannel){
                    InetAddress clientaddr;
                    Socket *clientsock=new Socket(servsock.accept(clientaddr));//堆上
                    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());
                    //ep.addfd(clientsock->fd(),EPOLLIN|EPOLLET|EPOLLRDHUP);
                    Channel *clientchannel =new Channel(&ep,clientsock->fd());
                    clientchannel->useet();
                    clientchannel->enablereading();

                }else{
                    char buf[1024];
                    while(1){
                        bzero(&buf, sizeof(buf));
                        ssize_t nread=read(ch->fd(),buf,1024);
                        if(nread>0){
                            printf("recv(eventfd=%d):%s\n",ch->fd(),buf);
                            send(ch->fd(),buf,nread,0);
                        }else if(nread==-1&&errno==EINTR){
                            //读取信号时信号中断
                            continue;
                        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                            break;
                        }
                        else if(nread==0){
                            printf("client(eventfd=%d) disconnected.\n",ch->fd());
                            epoll_ctl(ep.epollfd(), EPOLL_CTL_DEL, ch->fd(), NULL);
                            close(ch->fd());           
                            break;
                        }
                    }
                    
                }
            }
            else if(ch->revents()&EPOLLOUT){

            }else{//其他事件都为错误
                printf("client(eventfd=%d) error.\n",ch->fd());
                epoll_ctl(ep.epollfd(), EPOLL_CTL_DEL, ch->fd(), NULL);
                close(ch->fd()); 
            }
                
        }
    }
    return 0;
}
