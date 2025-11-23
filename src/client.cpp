#include<sys/socket.h>
#include<iostream>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<unistd.h>
int main(int argc, char const *argv[])
{
    if(argc!=3){
        std::cout<<"ip port"<<std::endl;
        return -1;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        std::cout<<"socket error"<<std::endl;
        return -1;
    }
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if(connect(sockfd,(sockaddr*)&servaddr,sizeof(servaddr))<0){
        std::cout<<"connect error"<<std::endl;
        return -1;
    }

    char buf[1024];
    std::cout<<"connect success"<<std::endl;
    for(int i=0;i<100000;i++){
        memset(buf, 0, sizeof(buf));
        std::cout<<"please input:"<<std::endl;
        std::cin>>buf;
        if(send(sockfd,buf,strlen(buf),0)<=0){
            std::cout<<"send error"<<std::endl;
            close(sockfd);
            return -1;
        }
        memset(buf, 0, sizeof(buf));
        if(recv(sockfd,buf,sizeof(buf),0)<=0){
            std::cout<<"recv error"<<std::endl;
            close(sockfd);
            return -1;
        }
        std::cout<<"recv: "<<buf<<std::endl;
    }
    return 0;
}
