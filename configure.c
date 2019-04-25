#include "client.h"

void configure(char* ip, char* port){
	int fp = open(".configure", O_CREAT | O_WRONLY, 600);
	printf("%d\n", fp);
	write(fp, ip, strlen(ip));
	write (fp, "\n", 1);
	write(fp, port, strlen(port));
}
