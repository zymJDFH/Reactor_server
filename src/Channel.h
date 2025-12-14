#pragma once
#include "EventLoop.h"
#include "sys/epoll.h"
#include "Socket.h"
#include <functional>
#include <memory>
class Epoll;
class EventLoop;
class Channel{//事件管理
private:
    int fd_=-1;
    const std::unique_ptr<EventLoop>& loop_;      //channel对应的fd
    bool inepoll_=false;    //channel是否已经添加到epoll树上
    uint32_t events_=0;  //fd_需要监视的事件
    uint32_t revents_=0; //fd_已发生的事件
  
    std::function<void()>readcallback_; //fd读事件的回调函数
    std::function<void()>closecallback_;
    std::function<void()>errorcallback_;
    std::function<void()>writecallback_;
public:
    Channel(const std::unique_ptr<EventLoop>&loop,int fd);
    ~Channel();

    int fd();
    bool inepoll();
    uint32_t events();
    uint32_t revents();
    void useet();

    void enablereading();
    void disablereading();
    void enablewriting();   //注册写事件
    void disablewriting();
    void disableall();      //取消全部事件
    void remove();          //从事件循环中删除Channel

    void setinepoll();
    void setrevents(uint32_t ev);
    void handleevent();
    
    void setreadcallback(std::function<void()>fn);
    void seterrorcallback(std::function<void()>fn);
    void setclosecallback(std::function<void()>fn);
    void setwritecallback(std::function<void()>fn);

};