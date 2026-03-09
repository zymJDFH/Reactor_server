#include "EventLoop.h"
int createtimerfd(int sec=30)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);   // 创建timerfd。
    struct itimerspec timeout;                                // 定时时间的数据结构。
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;                             // 定时时间，固定为5，方便测试。
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd,0,&timeout,0);
    return tfd;
}

EventLoop::EventLoop(bool mainloop,int timetvl,int timeout)
    :ep_(new Epoll),threadid_(0),mainloop_(mainloop),timetvl_(timetvl),timeout_(timeout),stop_(false)
    ,wakeupfd_(eventfd(0,EFD_NONBLOCK)),wakeupchannel_(new Channel(this,wakeupfd_)),
    timerfd_(createtimerfd(timeout)),timerchannel_(new Channel(this,timerfd_))
{
    wakeupchannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakeupchannel_->enablereading();
    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer,this));
    timerchannel_->enablereading();
}
EventLoop::~EventLoop(){
    //delete ep_;
}

void EventLoop::run(){
    threadid_=syscall(SYS_gettid);
    while(stop_==false){
        std::vector<Channel*>channels=ep_->loop(10*1000);
        if(channels.size()==0){
            if(epolltimeoutcallback_) epolltimeoutcallback_(this);
        }else{
            for(auto &ch:channels){
                ch->handleevent();
            }
        }

        if (!pendingcloseconns_.empty())
        {
            std::vector<spConnection> timeoutconns=std::move(pendingcloseconns_);
            pendingcloseconns_.clear();
            for (auto &conn:timeoutconns) conn->closecallback();
        }
    }
}
void EventLoop::stop(){
    stop_=true;
    wakeup();
}

void EventLoop::updatechannel(Channel*ch){
    ep_->updatechannel(ch);
}
void EventLoop::removechannel(Channel*ch){
    ep_->removechannel(ch);
}

//设置epollwait超时的回调函数
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)>fn){
    epolltimeoutcallback_=fn;
}
bool EventLoop::isinloopthread(){
    return threadid_==syscall(SYS_gettid);  
}

void EventLoop::queueinloop(std::function<void()>fn){
    {
        std::lock_guard<std::mutex>gd(mutex_);
        taskqueue_.push(fn);
    }
    //唤醒事件循环
    wakeup();
}
void EventLoop::wakeup(){
    uint64_t val=1;
    write(wakeupfd_,&val,sizeof(val));
}
void EventLoop::handlewakeup(){
    uint64_t val;
    read(wakeupfd_,&val,sizeof(val));   //从eventfd中读取数据，如果不读取，eventfd的读事件会一直触发
    std::queue<std::function<void()>> tasks;
    {
        std::lock_guard<std::mutex>gd(mutex_);
        std::swap(tasks,taskqueue_);
    }

    while(tasks.size()>0){
        std::function<void()> fn=std::move(tasks.front());
        tasks.pop();
        fn();
    }
}
void EventLoop::handletimer(){
    uint64_t expirations=0;
    ssize_t n=read(timerfd_,&expirations,sizeof(expirations));
    if ((n==-1) && (errno!=EAGAIN) && (errno!=EWOULDBLOCK) && (errno!=EINTR))
    {
        perror("read(timerfd_) failed");
    }

    struct itimerspec timeout;                                // 定时时间的数据结构。
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timetvl_;                             // 定时时间，固定为5，方便测试。
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_,0,&timeout,0);
    if(mainloop_) return;

    time_t now=time(0);
    std::vector<spConnection> timeoutconns;
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        for (auto &it:conns_)
        {
            if (it.second->timeout(now,timeout_))
            {
                timeoutconns.push_back(it.second);
            } 
        }
    }

    pendingcloseconns_.insert(pendingcloseconns_.end(),timeoutconns.begin(),timeoutconns.end());
}

void EventLoop::newconnection(spConnection conn){
    {
        std::lock_guard<std::mutex>gd(mmutex_);
        conns_[conn->fd()]=conn;    
    }
}
void EventLoop::removeconnection(int fd){
    std::lock_guard<std::mutex>gd(mmutex_);
    conns_.erase(fd);
}
void EventLoop::settimercallback(std::function<void(int)>fn){
    timercallback_=fn;
}
