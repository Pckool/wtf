#include "c_client.h"

void add(char* proj, char* file){
        int fd = open(file, O_RDWR); //open file
        if (fd < 0){
                printf("There was an error opening the given file...\nError No: %d\n", fd);
                return NULL;
        }
        char buffer[2000];
        read(fd, buffer, 2000); // read the entire file
        size_t length = strlen(buffer);
        unsigned char temp[SHA_DIGEST_LENGTH]; // temporary storage for hash

        unsigned char *hash = (char *)malloc( (SHA_DIGEST_LENGTH * 2) * sizeof(char) + 1); //actual hash goes here
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
        
        int man_fd;
        printf("Trying to open/create %s\n", mpath);
        man_fd = open(mpath, O_RDWR | O_APPEND); //Open manifest

        if (man_fd < 0){
                printf("There was an error opening .Manifest the file...\nError No: %d\n", man_fd);
                return NULL;
        }

        char contents[10000];
        read(man_fd, contents, 10000); //read manifest
        char *fileName = strstr(contents, file); //makes pointer to filename in the manifest contents if it can find it
        char *version = "1"; // The version number. This gets incremented if the number is found

        char *final;

        if (man_fd < 0){
                printf("There was an error opening the file...\n");
                return NULL;
        }

        if (fileName == NULL){ //If file isnt in the manifest
                printf("No .Manifest file found...\n");
                final = createaManLine(file, version, hash);
                printf("Created the line: %s...\n", final);
                // write(man_fd, file, strlen(file));

                // write(man_fd, "\t", 1);
                // write(man_fd, "1\t", 2);
                // write(man_fd, hash, SHA_DIGEST_LENGTH * 2);
                unsigned finalLineSize = strlen(file) + strlen(version) + (SHA_DIGEST_LENGTH * 2);
                int status = write(man_fd, final, finalLineSize);
                if (status != finalLineSize){
                        printf(".Manifest write was not successful...\nError No: %d\n", status);
                }
                else{
                        printf(".Manifest write was successful...\nWrote %d bytes...\n", status);
                }
                
        }
        else{ //If file is in manifest
                printf("Found a .Manifest...\n");
                final = createaManLine(file, version, hash);
                replaceLine(&contents, &fileName, final);
                /*
                char *ptr[2];
                ptr[0] = strtok(fileName, "\t");
                ptr[1] = strtok(NULL, "\t");
                ptr[2] = strtok(NULL, "\t");
                int version = atoi(ptr[1]);
                if (memcmp(temp, ptr[2], SHA_DIGEST_LENGTH) != 0){ //Compare new manifest to old manifest
                        int position = fileName - contents;
                        printf("%d\n", version);
                }
                */
               printf("content: %s\n", contents);
        }
        close(man_fd);
        free(hash);
}

// create a thing
char *createaManLine(char *file, char *version, char *hash){
        printf("file path: %s\n", file);
        char *file_t = concat(file, "\t");
        char *version_t = concat(version, "\t");

        char *part1 = concat(file_t, version_t);
        free(version_t);
        free(file_t);

        char *hash_t = concat(hash, "\t");
        // SHA_DIGEST_LENGTH * 2

        char *part2 = concat(part1, hash_t);

        free(part1);
        free(hash_t);
        char *final = concat(part2, "\n");

        return final;
}

char *replaceLine(char **content, char **line, char *newLine){
        strcpy(*line, newLine);
        printf("this is the line %s\n", *content);
}

char *removeLine(char *content, char **line){

}

