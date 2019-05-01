#include "client.h"

void add(char* proj, char* file){
	char path[2000];
	snprintf(path, 2000, "%s/%s", proj, file);
	int fd = open(path, O_RDWR);
	char buffer[2000];
	read(fd, buffer, 2000);
	printf("%s\n", buffer);
	size_t length = strlen(buffer);
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(buffer, length, hash);
	snprintf(path, 2000, "%s/%s", proj, ".Manifest");
	close(fd);
	fd = open(path, O_RDWR);
	write(fd, file, strlen(file));
	write(fd, "\t", 1);
	write(fd, hash, SHA_DIGEST_LENGTH);
	
}

int findDir(char *dirname){
    // returns a file descriptor of the file requested.
    // If it can find the file, it will return a fd, otherwise it will return a number < 0
    int fd;

    fd = open(dirname, O_RDWR, 600);

    return fd;
}
