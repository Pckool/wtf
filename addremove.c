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

        int man_fd = open(mpath, O_RDWR | O_APPEND); //Open manifest
        char contents[10000];
        read(man_fd, contents, 10000); //read manifest
        char *fileName = strstr(contents, file); //makes pointer to filename in the manifest contents if it can find it
        
        char *version = "1"; // The version number. This gets incremented if the number is found

        char *final;

        if (fileName == NULL){ //If file isnt in the manifest
                final = createaManLine(file, version, hash);
                /*
                char *file_t = stringAppend(file, "\t", strlen(file));

                char *version_t = stringAppend(version, "\t", strlen(version));

                char *part1 = stringAppend(file_t, version_t, strlen(file_t));
                free(version_t);
                free(file_t);

                char *hash_t = stringAppend(hash, "\t", SHA_DIGEST_LENGTH * 2);

                char *part2 = stringAppend(part1, hash_t, strlen(part1));

                free(part1);
                free(hash_t);
                char *final = stringAppend(part2, "\n", strlen(part2));
                */
                // write(man_fd, file, strlen(file));

                // write(man_fd, "\t", 1);
                // write(man_fd, "1\t", 2);
                // write(man_fd, hash, SHA_DIGEST_LENGTH * 2);
                
                write(man_fd, final, 1);
        }
        else{ //If file is in manifest
                final = createaManLine(file, version, hash);
                replaceLine(contents, &fileName, final);
                char *ptr[2];
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

// create a thing
char *createaManLine(char *file, char *version, char *hash){
        char *file_t = stringAppend(file, "\t", strlen(file));

        char *version_t = stringAppend(version, "\t", strlen(version));

        char *part1 = stringAppend(file_t, version_t, strlen(file_t));
        free(version_t);
        free(file_t);

        char *hash_t = stringAppend(hash, "\t", SHA_DIGEST_LENGTH * 2);

        char *part2 = stringAppend(part1, hash_t, strlen(part1));

        free(part1);
        free(hash_t);
        char *final = stringAppend(part2, "\n", strlen(part2));

        return final;
}

char *replaceLine(char *content, char *line, char *newLine){
        strcpy(line, newLine);
        printf("this is the line %s\n", line);
}

char *removeLine(char *content, char *line){

}

