all: client server
client: client.cpp
	g++ -o client client.cpp -lpthread
server: tcpepoll.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp
	g++ -o server tcpepoll.cpp Socket.cpp InetAddress.cpp Epoll.cpp Channel.cpp -lpthread
clean:
	rm -f client server	