#include "client.h"

void add(){
    // check for a manifest
    // if themanifestexists,
}

int findDir(char *dirname){
    // returns a file descriptor of the file requested.
    // If it can find the file, it will return a fd, otherwise it will return a number < 0
    int fd;

    fd = open(dirname, O_RDWR, 600);

    return fd;
}
