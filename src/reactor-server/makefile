all: client server
client: client.cpp
	g++ -o client client.cpp -lpthread
server: tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp
	g++ -o server tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp -lpthread
clean:
	rm -f client server	