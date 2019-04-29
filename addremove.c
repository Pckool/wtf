#include "client.h"

void add(char* proj, char* file){
	char path[2000];
	snprintf(path, 2000, "%s/%s", proj, ".Manifest");
	int fd = open(file, O_RDWR);
	char buffer[2000];
	read(fd, buffer, 2000);
	size_t length = strlen(buffer);
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(buffer, length, hash);
	int x = 0;
	for (x = 0; x < SHA_DIGEST_LENGTH; x++){
		printf("%02x", hash[x]);
	}
	putchar('\n');
	
}

int findDir(char *dirname){
    // returns a file descriptor of the file requested.
    // If it can find the file, it will return a fd, otherwise it will return a number < 0
    int fd;

    fd = open(dirname, O_RDWR, 600);

    return fd;
}
