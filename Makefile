CC=gcc
DEPS = global.h
# CFLAGS=

%.o: %.c $(DEPS)

Client: client.c
	$(CC) -o Client client.c

Server: server.c
	$(CC) -o Server server.c
