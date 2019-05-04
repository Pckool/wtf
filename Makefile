CC=gcc
S_DEPS = s_server.h
C_DEPS = c_client.h
H_DEPS = h_global.h
CFLAGS= -O

c_%.o: c_%.c $(C_DEPS) $(H_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

s_%.o: s_%.c $(S_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


h_%.o: h_%.c $(S_DEPS) $(H_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


all: Client Server

Client: c_client.o c_addremove.o c_configure.o h_helpers.o
	$(CC) -o  $@ c_client.o -lssl -lcrypto c_addremove.o c_configure.o h_helpers.o
client.o: c_client.h h_global.h

Server: s_server.o
	$(CC) -o $@ -pthread s_server.o h_helpers.o
server.o: c_server.h h_helpers.o h_global.h

clean:
	rm *.o Server Client
