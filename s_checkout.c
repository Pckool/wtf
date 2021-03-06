#include "h_both.h"
#include "s_server.h"
int sockfd_local = -1;

pthread_mutex_t mutextest = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_id_filePush;


// add a response protocol to let the client know if they can recieve or not

void checkout_s(const char *buffer, int sockfd){
    printf("Starting fetch routine with buffer %s...\n", buffer);
    sockfd_local = sockfd;
    char *proj = malloc(sizeof(buffer - 9)); //The reason its - 9 is because thats how many bytes "rmdir:" is.
    int i = 0;
    int p = 9;
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

                data->path = (char *)malloc(strlen(path) * sizeof(char) + 1);
                memcpy(data->path,"\0", strlen(path) * sizeof(char) + 1); // ensure it is a string
                strcpy(data->path, path);

                data->projectName = (char *)malloc(strlen(projectName) * sizeof(char) + 1);
                memcpy(data->projectName,"\0", strlen(projectName) * sizeof(char) + 1); // ensure it is a string
                strcpy(data->projectName, projectName);

                pthread_create(&thread_id_filePush, NULL, pushFileToClientoop, (void*)data);
                pthread_join(thread_id_filePush, NULL);
                // launch a thread and mutex it
                
            }
            
        }
        
    }
    return 0;
    (void) closedir(dir);
}


void *pushFileToClientoop(void *dat){
    printf("Sending file to client...\n");

    threadData *data = (threadData *)dat;

    char newPath_Man[PATH_MAX];
    snprintf(newPath_Man, PATH_MAX, "%s/%s", data->path, ".Manifest");

    char newPath_Dat[PATH_MAX];
    snprintf(newPath_Dat, PATH_MAX, "%s/%s", data->path, "data.tar.gz");


    int fd_data = open(newPath_Dat, O_RDWR);
    if(fd_data < 0){
        printf("there was a problem opening %s; aborting...\n", newPath_Dat);
        return;
    }

    int fd_man = open(newPath_Man, O_RDWR);
    if(fd_man < 0){
        printf("there was a problem opening %s; aborting...\n", newPath_Man);
        return;
    }


    // for manifest
    struct stat fileStat_man;
    if(fstat(fd_man, &fileStat_man) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    char project_buffer[fileStat_man.st_size];
    if(read(fd_man, project_buffer, fileStat_man.st_size) < 0){
        printf("There was an error reading file `%s`...\n", newPath_Dat);
    }
    // for data
    struct stat fileStat_dat;
    if(fstat(fd_data, &fileStat_dat) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    char manifest_buffer[fileStat_dat.st_size];
    if(read(fd_data, manifest_buffer, fileStat_dat.st_size) < 0){
        printf("There was an error reading file `%s`...\n", newPath_Man);
    }
    
    

    printf("\nRead data with %d bytes...\n\n", fileStat_dat.st_size);
    // char *clientPath = getClientsPath(data->path, data->projectName);
    char *message;
    int len = strlen(data->projectName) + sizeof("2") + sizeof("data.tar.gz") + sizeof("file:") + sizeof(".Manifest") ;
    message = (char *)malloc(len * sizeof(char));
    memcpy(message, "\0", len * sizeof(char));
    
    // char *byte_content = getByteContent(data->path);
    
    // snprintf(message, len, "file:%s:%s", data->projectName, byte_content);
    snprintf(message, len, "file:%s:%s:%s:%s", data->projectName, "2", "data.tar.gz", ".Manifest");

    
    
    printf("This is sending to the client: %s\n", message);

    if(write(sockfd_local, message, len) < 0){
        printf("There was an issue writing to the socket...\n");
        return;
    }
    printf("Message sent successfully...\n");
    // Sending data.tar.gz
    int amm = write(sockfd_local, project_buffer, fileStat_dat.st_size);
    if(amm < 0){
        printf("There was an issue writing to the socket...\n");
        return;
    }
    printf("File sent successfully with %d/%d bytes written...\n", amm, fileStat_dat.st_size);

    // sending .Manifest
    amm = write(sockfd_local, manifest_buffer, fileStat_man.st_size);
    if(amm < 0){
        printf("There was an issue writing to the socket...\n");
        return;
    }
    printf("File sent successfully with %d/%d bytes written...\n", amm, fileStat_man.st_size);
    

    close(fd_data);
}


int getProjectCurrVersion(char *ProjectName){
    printf("What is the latest version in %s?\n", ProjectName);
    DIR *dir;
    struct dirent *dp;
    int version = 0;
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", ".repo", ProjectName);


    if ((dir = opendir (path)) == NULL) {
        printf("ERROR failed to get version number; Cannot open path `%s`...\n", path);
        return;
    }
    while ((dp = readdir(dir) ) != NULL){
        if(dp->d_type == DT_DIR){
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                printf("skipping `%s`\n", dp->d_name);
                continue;
            }
            printf("Found Dir: `%s`\n", dp->d_name);
            ++version;
                
            
        }   
    }
    
    (void) closedir(dir);
    
    return version - 1;
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


