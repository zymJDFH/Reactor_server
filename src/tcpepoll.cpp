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
#include "EventLoop.h"
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

    EventLoop loop;
    Channel *servchannel =new Channel(loop.ep(),servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection,servchannel,&servsock));
    servchannel->enablereading();
    
    loop.run();
    return 0;
}
