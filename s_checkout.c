#include "h_both.h"
#include "s_server.h"
int sockfd_local = -1;

pthread_mutex_t mutextest = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_id_filePush;




void checkout_s(const char *buffer, int sockfd){
    printf("Starting fetch routine with buffer %s...\n", buffer);
    sockfd_local = sockfd;
    char *proj = malloc(sizeof(buffer - 9)); //The reason its - 9 is because thats how many bytes "rmdir:" is.
    int i = 0;
    int p = 9;
    int x = 0;
    while (i < sizeof(proj)){
        proj[i] = buffer[p]; //Transfer the project name from the buffer to proj.
        i++;
        p++;
    }
	int version = getProjectCurrVersion(proj);
    if(version < 0) return; // if there was an error accessing the metafile

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", ".repo", proj);

    printf("appending %s to .repo\n", proj);

    // convert version number into a string
    char version_str[10];
    snprintf(version_str, 10, "%d", version);

    // create a new path string with the version number folder
    char h_version_path[PATH_MAX];
    snprintf(h_version_path, PATH_MAX, "%s/%s", path, version_str);

    scanDir_sendFiles(h_version_path, sockfd, proj);
    
}

int scanDir_sendFiles(char *path, int sockfd, char *projectName){
    printf("Scanning to find the file(s) in `%s` to send to the user.\n", path);
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) == NULL) {
        printf("Cannot open path %s...\n", path);
        return;
    }
    while ((dp = readdir(dir) ) != NULL){
        // this if statement doesn't work. d_type doesn't exist sadly :(
        if (dp->d_type == DT_DIR){ // this is a dir, so we need to loop through this as well
            printf("A directory `%s` has been found...\n", dp->d_name);
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                printf("skipping `%s`...\n", dp->d_name);
                continue;
            }
            char newPath[PATH_MAX];
            snprintf(newPath, PATH_MAX, "%s/%s", path, dp->d_name);

            scanDir_sendFiles(newPath, sockfd, projectName);
        }
        else{ // this is a file, so send it off to the client
            printf("A file `%s` has been found...\n", dp->d_name);

            if(strcmp(dp->d_name, "data.tar.gz") == 0){
                char newPath[PATH_MAX];
                snprintf(newPath, PATH_MAX, "%s/%s", path, dp->d_name);

                int fd = open(newPath, O_RDWR);
                if(fd < 0) {
                    printf("Something went wrong opening `%s`\n", newPath);
                    return -1;
                }
                close(fd);
                printf("This is the appended path: `%s`\n", newPath);
                threadData *data = (threadData *)malloc(sizeof(threadData));

                data->path = (char *)malloc(strlen(newPath) * sizeof(char) + 1);
                memcpy(data->path,"\0", strlen(newPath) * sizeof(char) + 1); // ensure it is a string
                strcpy(data->path, newPath);

                data->projectName = (char *)malloc(strlen(projectName) * sizeof(char) + 1);
                memcpy(data->projectName,"\0", strlen(projectName) * sizeof(char) + 1); // ensure it is a string
                strcpy(data->projectName, projectName);

                pthread_create(&thread_id_filePush, NULL, pushFileToClient, (void*)data);
                pthread_join(thread_id_filePush, NULL);
                // launch a thread and mutex it
                
            }
            
        }
        
    }
    return 0;
    (void) closedir(dir);
}


void *pushFileToClient(void *dat){
    printf("Sending file to client...\n");
    threadData *data = (threadData *)dat;
    int fd = open(data->path, O_RDWR);

    if(fd < 0){
        printf("there was a problem opening %s; aborting...\n", data->path);
        return;
    }


    struct stat fileStat;
    printf("This is the data:\tfd: %d\tpath: %s\n", fd, data->path);
    if(fstat(fd, &fileStat) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    char buffer[fileStat.st_size];
    if(read(fd, buffer, fileStat.st_size) < 0){
        printf("There was an error reading file `%s`...\n", data->path);
    }
    

    printf("\nRead data with %d bytes...\n\n", fileStat.st_size);
    // char *clientPath = getClientsPath(data->path, data->projectName);
    char *message;
    int len = strlen(data->projectName) + sizeof("1") +sizeof("data.tar.gz") + sizeof("file:");
    message = (char *)malloc(len * sizeof(char));
    memcpy(message, "\0", len * sizeof(char));
    
    char *byte_content = getByteContent(data->path);
    
    // snprintf(message, len, "file:%s:%s", data->projectName, byte_content);
    snprintf(message, len, "file:%s:%s:%s", data->projectName, "1", "data.tar.gz");

    
    
    printf("This is sending to the client: %s\n", message);

    if(write(sockfd_local, message, len) < 0){
        printf("There was an issue writing to the socket...\n");
        return;
    }
    printf("Message sent successfully...\n");
    int amm = write(sockfd_local, buffer, fileStat.st_size);
    if(amm < 0){
        printf("There was an issue writing to the socket...\n");
        return;
    }
    
    printf("File sent successfully with %d/%d bytes written...\n", amm, fileStat.st_size);

    close(fd);
}


int getProjectCurrVersion(char *ProjectName){
    printf("What is the latest version?\n");
    DIR *dir;
    struct dirent *dp;
    int version = 0;
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", ".repo", ProjectName);


    if ((dir = opendir (path)) == NULL) {
        printf("Cannot open path `%s`...\n", path);
        return;
    }
    while ((dp = readdir(dir) ) != NULL){
        if(dp->d_type == DT_DIR){
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                printf("skipping `%s`...\n", dp->d_name);
                continue;
            }
            printf("Found Dir: `%s`\n", dp->d_name);
            ++version;
                
            
        }   
    }
    
    (void) closedir(dir);
    
    return version;
}

// this will only be used if we decide to compress files one at a time. If we go the tar route, this function will not be used.
char *getClientsPath(char *serverPath, char *projectName){
    printf("Figuring out the correct path...\n");
    int versionNo = getProjectCurrVersion(projectName);
    char *serverPath_cpy = malloc(strlen(serverPath) * sizeof(char));
    memcpy(serverPath_cpy, "\0", (strlen(serverPath) * sizeof(char)) );

    memcpy(serverPath_cpy, serverPath, (strlen(serverPath) * sizeof(char)) );

    char *Version_str[10];
    memcpy(Version_str, "\0", (10) );

    snprintf(Version_str, 10, "%d", versionNo);

    strcpy(serverPath_cpy, serverPath);
    removeSubstring(serverPath_cpy, ".repo/");

    removeSubstring(serverPath_cpy, ".repo/");
    removeSubstring(serverPath_cpy, Version_str);
    return serverPath_cpy;
}


