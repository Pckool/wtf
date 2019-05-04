CC=gcc
DEPS = global.h client.h server.h
CFLAGS= -O

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)





all: Client Server

Client: client.o addremove.o configure.o helpers.o
	$(CC) -o  $@ client.o -lssl -lcrypto addremove.o configure.o helpers.o
client.o: client.h global.h

Server: server.o
	$(CC) -o $@ -pthread server.o
server.o: server.h global.h

clean:
	rm *.o Server Client
