#include "c_client.h"

void configure(char* ip, char* port){
	int fd = open(".configure", O_WRONLY | O_CREAT | O_TRUNC, 0600); 
	write(fd, ip, strlen(ip));
	write (fd, "\n", 1);
	write(fd, port, strlen(port));
}
