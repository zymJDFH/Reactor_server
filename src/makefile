all: client echoserver
client: client.cpp
	g++ -g -o client client.cpp -lpthread
echoserver: echoserver.cpp EchoServer.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp  Acceptor.cpp Connection.cpp Buffer.cpp ThreadPool.cpp
	g++ -g -o echoserver echoserver.cpp EchoServer.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp  EventLoop.cpp TcpServer.cpp Acceptor.cpp Buffer.cpp Connection.cpp ThreadPool.cpp -lpthread
clean:
	rm -f client echoserver	