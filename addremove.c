#include "client.h"

void add(char* proj, char* file){
        int fd = open(file, O_RDWR); //open file
        char buffer[2000];
        read(fd, buffer, 2000); // read the entire file
        size_t length = strlen(buffer);
        unsigned char temp[SHA_DIGEST_LENGTH]; // temporary storage for hash
        unsigned char* hash = malloc((SHA_DIGEST_LENGTH)*sizeof(char)); //actual hash goes here
        /* The next seven lines I found online and show you how to  format a hash to be written properly to a file */
        memset(temp, 0x0, SHA_DIGEST_LENGTH);
        memset(hash, 0x0, SHA_DIGEST_LENGTH);
        SHA1((const char*)buffer, (unsigned long)strlen(buffer), temp);
        int x;
        for(x = 0; x < SHA_DIGEST_LENGTH; x++){
                sprintf((char*)&(hash[x*2]), "%02x", temp[x]);
        }
        char* mpath[2000];
        snprintf(mpath, 2000, "%s/%s", proj, ".Manifest"); //Path to manifest
        close(fd);
        fd = open(mpath, O_RDWR); //Open manifest
        char contents[10000];
        read(fd, contents, 10000); //read manifest
        char *fileName = strstr(contents, file); //makes pointer to filename in the manifest contents if it can find it
        if (fileName == NULL){ //If file isnt in the manifest
                write(fd, file, strlen(file));
                write(fd, "\t", 1);
                write(fd, "1\t", 2);
                write(fd, hash, SHA_DIGEST_LENGTH * 2);
                write(fd, "\n", 1);
        }
        else{ //If file is in manifest
                char* ptr[2];
                ptr[0] = strtok(fileName, "\t");
                ptr[1] = strtok(NULL, "\t");
                ptr[2] = strtok(NULL, "\t");
                int version = atoi(ptr[1]);
                if (memcmp(temp, ptr[2], SHA_DIGEST_LENGTH) != 0){ //Compare new manifest to old manifest
                        int position = fileName - contents;
                        printf("%d\n", version);
                }
        }
}


int findDir(char *dirname){
    // returns a file descriptor of the file requested.
    // If it can find the file, it will return a fd, otherwise it will return a number < 0
    int fd;

    fd = open(dirname, O_RDWR, 600);

    return fd;
}
