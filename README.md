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
