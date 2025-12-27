#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <functional>
#include <memory>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <queue>
#include <mutex>
#include <map>
#include <sys/timerfd.h>
#include "Connection.h"
#include <atomic>
class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;
class EventLoop{
private:
    std::unique_ptr<Epoll>ep_;
    std::function<void(EventLoop*)>epolltimeoutcallback_;
    pid_t threadid_;
    std::queue<std::function<void()>>taskqueue_;    //事件循环线程被eventfd唤醒后执行的任务队列

    std::mutex mutex_;
    int wakeupfd_;          //用于唤醒事件循环线程的eventfd
    std::unique_ptr<Channel> wakeupchannel_;
    int timerfd_;   //定时器的fd
    std::unique_ptr<Channel>timerchannel_;
    bool mainloop_; 
    std::mutex mmutex_;     //保护conns_的互斥锁
    std::map<int,spConnection>conns_;   //存放运行在该事件循环上的全部Connection对象
    std::function<void(int)>timercallback_;
    int timetvl_;
    int timeout_;
    std::atomic_bool stop_;
public:
    EventLoop(bool mainloop,int timetvl=30,int timeout=80);
    ~EventLoop();
    void run();
    void stop();

    void updatechannel(Channel*ch);
    void removechannel(Channel*ch);
    void setepolltimeoutcallback(std::function<void(EventLoop*)>fn);
    bool isinloopthread(); //判断当前线程是否为该事件循环的线程
    void queueinloop(std::function<void()>fn);  //把任务添加到队列中
    void wakeup(); //唤醒事件循环
    void handlewakeup();
    void handletimer();
    void newconnection(spConnection conn);  //把Connection对象存放在conns_中
    void settimercallback(std::function<void(int)>fn);
};