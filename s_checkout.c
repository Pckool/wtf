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
    printf("Scanning to find the file(s) in %s to send to the user.\n", path);
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) == NULL) {
        printf("Cannot open path %s...\n", path);
        return;
    }
    while ((dp = readdir(dir) ) != NULL){
        // this if statement doesn't work. d_type doesn't exist sadly :(
        if (dp->d_type == DT_DIR){ // this is a dir, so we need to loop through this as well
            printf("A directory %s has been found...\n", dp->d_name);
            char newPath[PATH_MAX];
            snprintf(newPath, PATH_MAX, "%s/%s", path, dp->d_name);

            scanDir_sendFiles(newPath, sockfd, projectName);
        }
        else{ // this is a file, so send it off to the client
            printf("A file %s has been found...\n", dp->d_name);

            if(strcmp(dp->d_name, "data.tar.gz") == 0){
                char newPath[PATH_MAX];
                snprintf(newPath, PATH_MAX, "%s/%s", path, dp->d_name);

                int fd = open(newPath, O_RDWR);

                threadData *data = (threadData *)malloc(sizeof(threadData));
                data->fd = fd;

                data->path = (char *)malloc(strlen(newPath) * sizeof(char));
                memcpy(data->path,"\0", strlen(newPath) * sizeof(char)); // ensure it is a string
                strcpy(data->path, newPath);

                data->projectName = (char *)malloc(strlen(projectName) * sizeof(char));
                memcpy(data->projectName,"\0", strlen(projectName) * sizeof(char)); // ensure it is a string
                strcpy(data->projectName, projectName);

                pthread_create(&thread_id_filePush, NULL, pushFileToClient, (void*)&data);
                pthread_join(thread_id_filePush, NULL);
                // launch a thread and mutex it
            }
            
        }
        
    }
    (void) closedir(dir);
}


void *pushFileToClient(void *dat){
    printf("Sending file to client...\n");
    threadData *data = (threadData *)dat;
    struct stat fileStat;
    if(fstat(data->fd, &fileStat) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    char *buffer[fileStat.st_size];

    if(read(data->fd, buffer, fileStat.st_size) < 0){
        printf("There was an error reading file %s...\n", data->path);
    }
    
    // char *clientPath = getClientsPath(data->path, data->projectName);
    char *message;
    int len = strlen(data->projectName) + strlen(buffer) + strlen("file:") + 1;
    // message
    snprintf(message, len, "file:%s:%s", data->projectName, buffer);
    write(sockfd_local, message, len);
}


int getProjectCurrVersion(char *ProjectName){
    printf("What is the latest version?\n");
    DIR *dir;
    struct dirent *dp;
    int version = 0;
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", ".repo", ProjectName);


    if ((dir = opendir (path)) == NULL) {
        printf("Cannot open path %s...\n", path);
        return;
    }
    while ((dp = readdir(dir) ) != NULL){
        if(dp->d_type == DT_DIR){
            if(strcmp(dp->d_name, ".") == 0 && strcmp(dp->d_name, "..") == 0){
                printf("skipping %s...", dp->d_name);
                continue;
            }
            else{
                printf("Found Dir: %s with a difference of %d from '.'\n", dp->d_name, strcmp(dp->d_name, "."));
                ++version;
            }
                
            
        }   
    }
    
    (void) closedir(dir);
    
    return version;
}

// this will only be used if we decide to compress files one at a time. If we go the tar route, this function will not be used.
char *getClientsPath(char *serverPath, char *projectName){
    printf("Figuring out the correct path...");
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


char *getLine(char *str){
    printf("Extracting the first line from %s\n", str);
    char *temp = malloc(sizeof(char));
    memcpy(temp, "\0", sizeof(char));

    char *final = malloc(sizeof(char));
    memcpy(final, "\0", sizeof(char));
    char lett;
    int i;
    for(i=0; i<strlen(str); ++i){

        lett = str[i];
        temp = charAppend(temp, lett);

        if(lett == '\n'){ // if the letter is a new line terminator then we exit the loop
            i = strlen(str) - 1;
        }
    }
    return temp;
}