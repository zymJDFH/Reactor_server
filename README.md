# 高性能Reactor网络库 
## 一、项目介绍

### 1.1 项目概述
基于 C++11 实现的高性能 TCP 服务器，采用经典**主从 Reactor 架构**（One Loop Per Thread），结合 epoll 多路复用、非阻塞 IO、线程池、智能指针等核心技术，解决单线程 IO 瓶颈问题，支持高并发客户端连接。

服务器核心定位为“轻量且高效的 IO 事件驱动框架”，职责边界清晰：MainReactor 专注接收新连接，SubReactor 并行处理客户端 IO 交互，通过回调机制和解耦设计，兼顾并发性能、资源安全性与可扩展性，可作为各类 TCP 应用（如回显服务、协议网关）的基础骨架。

### 1.2 快速开始
#### 1.2.1 环境要求
- 操作系统：Linux（推荐 Ubuntu 18.04+ / CentOS 7+）
- 编译工具：GCC 7.0+（需支持 C++11 及以上标准）
- 依赖项：无第三方库，仅依赖 C++ 标准库与 Linux 系统调用

#### 1.2.2 编译与启动
```bash
# 进入项目根目录
cd reactor-server

# 清理编译产物并重新编译
make clean && make

# 启动服务器
./echoserver 127.0.0.1 8888 

# 启动客户端
sh ./test.sh
./client 127.0.0.1 8888
```


## 二、架构设计
### 整体架构：主从 Reactor 模型（One Loop Per Thread）
本项目采用**经典主从 Reactor 架构**，核心设计思想为 **One Loop Per Thread**（一个事件循环对应一个独立线程），通过将「新连接接收」与「客户端 IO 处理」解耦，解决单线程 IO 瓶颈问题，实现高并发场景下的性能最大化。

架构核心分为**主 Reactor（MainReactor）** 和**从 Reactor（SubReactor）** 两层，配合线程池实现 IO 任务的并行处理，架构图如下：


```ascii
                      +-------------------+
                      |     多客户端       |
                      +-------------------+
                                 |
                                 v
                  +-----------------------------+
                  |        MainReactor          |  
                  | - Acceptor (listenfd)       |
                  | - epoll_wait                |
                  | - accept新连接               |
                  +-----------------------------+
                                 |
                  新连接轮询分发
                                 |
          +----------------------+----------------------+
          |                      |                      |
          v                      v                      v
+---------------+       +---------------+       +---------------+
| SubReactor 1  |       | SubReactor 2  |       | SubReactor N  |
| - EventLoop   |       | - EventLoop   |       | - EventLoop   |
| - epoll_wait  |       | - epoll_wait  |       | - epoll_wait  |
| - Channel     |       | - Channel     |       | - Channel     |
| - Connection  |       | - Connection  |       | - Connection  |
+---------------+       +---------------+       +---------------+
          |                      |                      |
          +----------+-----------+----------------------+
                     |           |
                     v           v
              +-------------------------+
              |   业务线程池       |
              |   - 异步处理OnMessage    |
              +-------------------------+
```

## 三、模块设计
### InetAddress
```bash
InetAddress是对底层sockaddr_in结构的RAII式封装，用来统一表示一个网络地址（IPv4地址+端口）。
作用：1.存储和管理 sockaddr_in addr_
      2.提供便捷的构造、查询和转换方法

```
### Socket
```bash
Socket 类是对原始 socket 文件描述符的RAII封装，负责socket的创建、配置、绑定、监听、接受等全部生命周期操作。
作用：1.确保fd在析构时自动close
      2.设置socket常用选项
      3.提供面向对象的接口 ( bind accept listen...)
```
### Channel
```bash
Channel是事件分发和管理的核心桥梁，负责将fd与EventLoop关联起来，实现事件驱动编程。
作用:  1.封装fd和事件(events_,revents_)
       2.事件注册与更新
       3.事件处理与回调

```
### Epoll
```bash
Epoll用来封装epoll的系统调用。
作用：1.创建epoll实例
      2.管理Channel的注册
      3.等待就绪事件
      4.高性能支撑（ET）

```
### EventLoop
```bash
事件循环核心，One Loop Per Thread
作用：1.支持eventfd（跨线程唤醒），timerfd（超时检测）
      2.MainLoop专责accept，SubLoop处理IO
```
### Acceptor
```bash
在主事件循环中负责监听并接受客户端新连接，将新连接分发给TcpServer处理。
```
### Connection
```bash
单个客户端连接的完整封装。
作用：1.连接资源管理：RAII 封装客户端 socket（unique_ptr<Socket>）、Channel、输入/输出 Buffer
      2.事件处理及回调
      3.数据发送
      4.超时检测
```
### TcpServer
```bash
项目的框架，上接回显，下接底层组件。
作用：1.管理MainLoop、多个SubLoop和线程池。
      2.设置各种回调（新连接、消息、关闭、超时等）
      3.新连接到来时轮询分配到SubLoop
```
### Buffer
```bash
解决网络编程中的粘包/半包问题
```
### ThreadPool
```bash
1.IO线程池（TcpServer）,负责传输数据
2.工作线程池 (EchoServer)，负责处理数据（添加前缀、解析协议、数据库查询...）
```
### Timestamp
```bash
轻量级时间戳工具，用来记录时间点，并提供方便的查询和格式化功能。
作用：1.连接超时监测（Connection，EventLoop）
      2.日志打印
```
### EchoServer
```bash
属于应用层，直接使用TcpServer实现回显业务，可以配置subreactor数目和工作线程数目
```



