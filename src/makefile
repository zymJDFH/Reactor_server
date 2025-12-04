all: client server
client: client.cpp
	g++ -o client client.cpp -lpthread
server: tcpepoll.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp  Acceptor.cpp Connection.cpp Buffer.cpp
	g++ -o server tcpepoll.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp  EventLoop.cpp TcpServer.cpp Acceptor.cpp Buffer.cpp Connection.cpp -lpthread
clean:
	rm -f client server	