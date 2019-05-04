CC=gcc
S_DEPS = global.h server.h
C_DEPS = global.h client.h
CFLAGS= -O

c_%.o: c_%.c h_%.c $(C_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

s_%.o: s_%.c h_%.c $(S_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


all: Client Server

Client: c_client.o c_addremove.o c_configure.o c_helpers.o
	$(CC) -o  $@ c_client.o -lssl -lcrypto c_addremove.o c_configure.o c_helpers.o
client.o: c_client.h h_global.h

Server: s_server.o
	$(CC) -o $@ -pthread s_server.o
server.o: c_server.h h_global.h

clean:
	rm *.o Server Client
