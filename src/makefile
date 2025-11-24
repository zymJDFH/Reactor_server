all: client server
client: client.cpp
	g++ -o client client.cpp -lpthread
server: tcpepoll.cpp
	g++ -o server tcpepoll.cpp -lpthread
clean:
	rm -f client server	