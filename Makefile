CC=gcc
DEPS = global.h client.h server.h
CFLAGS=-O

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)





all: Client Server

Client: client.o addremove.o
	$(CC) -o $@ $^
client.o: client.h global.h

Server: server.o
	$(CC) -o $@ server.o
server.o: server.h global.h
	
clean:
	rm *.o Server Client
