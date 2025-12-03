#include "Connection.h"
Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock){
    clientchannel_ =new Channel(loop_,clientsock->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage,clientchannel_));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));//?
    clientchannel_->useet();
    clientchannel_->enablereading();
}
Connection::~Connection(){
    delete clientchannel_;
    delete clientsock_;
}
int Connection::fd() const{
    return clientsock_->fd();
}
std::string Connection::ip()const{
    return clientsock_->ip();
}
uint16_t Connection::port()const{
    return clientsock_->port();
}

void Connection::closecallback(){
    // printf("client(eventfd=%d) disconnected.\n",fd());
    // close(fd());
    closecallback_(this);
}
void Connection::errorcallback(){
    errorcallback_(this);
}

void Connection::setclosecallback(std::function<void(Connection*)>fn){
    closecallback_=fn;
}
void Connection::seterrorcallback(std::function<void(Connection*)>fn){
    errorcallback_=fn;
}