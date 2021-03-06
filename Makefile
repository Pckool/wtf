CC=gcc
S_DEPS = s_server.h
C_DEPS = c_client.h
H_DEPS = h_global.h h_both.h
CFLAGS= -O

c_%.o: c_%.c $(C_DEPS) $(H_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

s_%.o: s_%.c $(S_DEPS) $(H_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


h_%.o: h_%.c $(H_DEPS) # $(S_DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)


all: WTFClient WTFServer

WTFClient: c_client.o h_helpers.o c_addremove.o c_configure.o c_checkout.o c_compression.o c_push.o c_commit.o
	$(CC) -o  $@ -lssl -lcrypto $^
client.o: c_client.h $(H_DEPS)


WTFServer: h_helpers.o s_server.o s_destroy.o s_checkout.o s_currver.o s_rollback.o s_commit.o
	$(CC) -o $@ -pthread $^
server.o: s_server.h $(H_DEPS)


clean:
	rm *.o WTFServer WTFClient
