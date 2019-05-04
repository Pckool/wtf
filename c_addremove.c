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
        
        close(fd);

        char* mpath[2000];
        snprintf(mpath, 2000, "%s/%s", proj, ".Manifest"); //Path to manifest

        int man_fd;
        printf("Trying to open/create %s\n", mpath);
        man_fd = open(mpath, O_RDWR | O_APPEND); //Open manifest

        if (man_fd < 0){
                printf("There was an error opening .Manifest the file...\nError No: %d\n", man_fd);
                return NULL;
        }

        char contents[1000000];
        read(man_fd, contents, 1000000); //read manifest
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
                unsigned finalLineSize = strlen(final) + 1; // +1 for the null terminator
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
                unsigned finalLineSize = strlen(final) + 1; // +1 for the null terminator
                replaceLine(&contents, &fileName, final);
                printf("Replaced the line...\n");
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

void c_remove(char *proj, char *file){
         char* mpath[2000];
        snprintf(mpath, 2000, "%s/%s", proj, ".Manifest"); //Path to manifest

        int man_fd;
        printf("Trying to open/create %s\n", mpath);
        man_fd = open(mpath, O_RDWR | O_APPEND); //Open manifest

        if (man_fd < 0){
                printf("There was an error opening .Manifest the file...\nError No: %d\n", man_fd);
                return NULL;
        }

        char *contents = malloc(1000000);
        int written = read(man_fd, contents, 1000000); //read manifest

        char *fileName = strstr(contents, file); //makes pointer to filename in the manifest contents if it can find it
        char *version = "1"; // The version number. This gets incremented if the number is found

        char *final;

        if (man_fd < 0){
                printf("There was an error opening the file...\n");
                return NULL;
        }

        if (fileName != NULL){ //If file is in manifest
                printf("Found a .Manifest...\n");
                // final = createaManLine(file, version, hash);
                unsigned finalLineSize = strlen(final) + 1; // +1 for the null terminator
                removeLine(&contents, &fileName);
                printf("Removed the line...\n");
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
        printf("this is the line %s\n", *line);
        printf("this is the new line %s\n", newLine);
        char *restOfCont = strstr(*line,"\n");


        unsigned lineLen = strlen(newLine);
        printf("Line Length: %d\n", lineLen);

        char *oldLine = (char *)malloc(lineLen + 1 * sizeof(char));
        memcpy(oldLine, "\0", lineLen);
        oldLine = *line;

        printf("The old line: %s\n", oldLine);
        memcpy(*line, newLine, lineLen);
        // printf("this is the line %s\n", *content);
}


int removeLine(char **content, char **line){
        char *restOfCont = strstr(*line, "\n"); // gets the substring of content minus the line and everything before it
        char *restOfCont = ++(&restOfCont);
        char *lineOnly = line - &restOfCont; // this should be a pointer to THE LINE AND ONLY THE LINE to use it, to use it refrence lineOnly

        printf("This is the line: %s\n", lineOnly);
        
        char *contentCpy = malloc(strlen(*content) * sizeof(char));
        memcpy(contentCpy, "\0", strlen(*content) * sizeof(char));

        // char *newContent = malloc( (strlen(content)-lineOnly) * sizeof(char));
        // memcpy(newContent, "\0", (strlen(content)-lineOnly) * sizeof(char));

        // strcpy(contentCpy, );
}

manLineTokens *tokenizeLine(char *line){
        unsigned len = strlen(line);
        char *path = "";
        char *version = "";
        char *hash = "";

        int part = 0;

        struct manLineTokens *tokLine = malloc(sizeof(manLineTokens));

        int i;
        for(i = 0; i <= len; i++){
                // once we find \t then save that as a temp bit and switch side=1
		if(line[i] == '\t' && part == 0 && i != 0){
			// ++i;
			part = 1;
		}
                if(line[i] == '\t' && part == 1 && i != 0){
			// ++i;
			part = 2;
		}
		else if(line[i] == '\n' && part == 2 && i != 0){
                        tokLine->path = malloc(sizeof(char *));
                        tokLine->version = malloc(sizeof(char *));
                        tokLine->hash = malloc(sizeof(char *));

			tokLine->path = path;
                        tokLine->version = version;
                        tokLine->hash = hash;
			free(path);
			free(version);
                        free(hash);
			part = 0;
		}
		// if not a \t or a \n then save into one string with charAppend()
		else{
                        switch (part){
                                case 0:
                                        path = concat(path, line[i]);
                                        break;
                                case 1:
                                        version = concat(version, line[i]);
                                        break;
                                case 2:
                                        hash = concat(hash, line[i]);
                                        break;
                                default:
                                        break;
                        }
			// if(part == 0){
				
			// }
			// // looking for token
			// else if(part == 1){
				
			// }
                        // else if(part == 2){
				
			// }
		}
        }
        return mainLine;
}
