CC=gcc
DEPS = global.h client.h srv.h
CFLAGS=-O

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

all: Client Server

Client: client.c addremove.c clnt.h
	$(CC) -o Client client.c

Server: server.c srv.h
	$(CC) -o Server server.c
