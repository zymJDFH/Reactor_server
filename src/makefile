all: client server
client: client.cpp
	g++ -o client client.cpp -lpthread
server: tcpepoll.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp EventLoop.cpp
	g++ -o server tcpepoll.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp  EventLoop.cpp -lpthread
clean:
	rm -f client server	