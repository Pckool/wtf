CC=gcc
DEPS = global.h client.h srv.h
CFLAGS=-O

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

all: Client Server

Client: client.o addremove.o
	$(CC) -o $@ client.c

Server: server.o
	$(CC) -o $@ server.c

clean:
	rm *.o Server Client
