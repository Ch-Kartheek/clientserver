all: server client

runclient:
	./client

runserver: server
	./server

client: client.o
	g++ client.o -o client

client.o: client.cpp
	g++ -c client.cpp -o client.o

server: server.o
	g++ server.o -o server -l sqlite3

server.o: server.cpp
	g++ -c server.cpp -o server.o

clean:
	rm -f server.o client.o server client
