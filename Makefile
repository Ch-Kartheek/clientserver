all: server client

runclient:
	./client

runserver: server
	./server

client: client.o
	g++ client.o -o client

server: server.o
	g++ server.o -o server -l sqlite3

%.o: %.cpp
	g++ -c $< -o $@

# Check if sqlite3 executable exists
ifeq (, $(shell which sqlite3))
    $(error "sqlite3 not found in PATH. Please install SQLite3.")
endif

clean:
	rm -f *.o server client

