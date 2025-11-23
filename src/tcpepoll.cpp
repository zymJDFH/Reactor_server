#include<sys/socket.h>
#include<iostream>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<arpa/inet.h>  
#include<unistd.h>
#include<cstring>
#include <netinet/tcp.h>  
int main(int argc, char const *argv[])
{
     if(argc!=3){
        std::cout<<"ip port"<<std::endl;
        return -1;
    }
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenfd<0){
        perror("socket() failed");
        return -1;
    }

    // 设置listenfd的属性
    int opt = 1; 
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));    // 端口重用 防止端口处于time_wait 不能bind
    setsockopt(listenfd,SOL_SOCKET,TCP_NODELAY   ,&opt,static_cast<socklen_t>(sizeof opt));    // 关闭Nagle 可传输小数据包
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT ,&opt,static_cast<socklen_t>(sizeof opt));    
    setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE   ,&opt,static_cast<socklen_t>(sizeof opt)); 

    if(bind(listenfd,(sockaddr*)&servaddr,sizeof(servaddr))<0){
        perror("bind() failed");
        close(listenfd);
        return -1;
    }
    if(listen(listenfd,128)!=0){
        perror("listen() failed"); 
        close(listenfd);
        return -1;  
    }
    //创建epoll句柄
    int epfd=epoll_create(1);

    epoll_event ev;
    ev.events=EPOLLIN;//listenfd水平触发（持续触发）
    ev.data.fd=listenfd;

    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    //存放epoll_wait返回事件的数组
    struct epoll_event evs[1024];
    while(1){
        int nready=epoll_wait(epfd,evs,1024,-1); //监测epfd中的所有fd
        if(nready<0){
            perror("epoll_wait() failed");
            break;
        }
        if(nready==0){
           std::cout<<"epoll_wait() timeout."<<std::endl;;
           continue;
        }

        for(int i=0;i<nready;i++){
            if(evs[i].data.fd==listenfd){
                sockaddr_in clientaddr;
                socklen_t len=sizeof(clientaddr);
                int connfd=accept4(listenfd,(sockaddr*)&clientaddr,&len,SOCK_NONBLOCK);
                if(connfd<0){
                    std::cout<<"accept error"<<std::endl;
                    continue;
                }
                printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",connfd,inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
                ev.events=EPOLLIN|EPOLLET;//减少事件触发次数 ，降低cpu开销
                ev.data.fd=connfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);

            }else{
                char buf[1024];
                while(1){
                    bzero(&buf, sizeof(buf));
                    ssize_t n=read(evs[i].data.fd,buf,1024);
                    if(n<0){
                        std::cout<<"read error"<<std::endl;
                        continue;
                    }else if(n==0){
                        std::cout<<"client closed"<<std::endl;
                        epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
                        close(evs[i].data.fd);
                    }else{
                        std::cout<<"recv "<<n<<" bytes:"<<std::string(buf,n)<<std::endl;
                        send(evs[i].data.fd,buf,strlen(buf),0);
                    }
                }
                
            }
                
        }
    }


    return 0;
}
