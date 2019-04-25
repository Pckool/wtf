CC=gcc
DEPS = global.h client.h server.h
CFLAGS=-O

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

all: Client Server

Client: client.o addremove.o
	$(CC) -o $@ $^

Server: server.o
	$(CC) -o $@ server.o

clean:
	rm *.o Server Client
