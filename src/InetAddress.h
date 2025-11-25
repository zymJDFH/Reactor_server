#pragma once
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include<string>
#include<unistd.h>
//socket地址协议类
class InetAddress
{
private:
    sockaddr_in addr_;    
public:
    InetAddress();
    InetAddress(const std::string &ip,uint16_t port);//监听
    InetAddress(const sockaddr_in addr);//客户端
    ~InetAddress();

    const char*ip()const;
    uint16_t port()const;
    const sockaddr*addr()const;
    void setAddr(sockaddr_in clientaddr);
};


