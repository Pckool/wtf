#include "h_both.h"
#include "s_server.h"
int sockfd_local = -1;

pthread_mutex_t mutextest = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_id_filePush;

typedef struct threadData{
    int fd;
    char *path;
}threadData;


void checkout_s(const char *buffer, int sockfd){
    sockfd_local = sockfd;
    char *proj = malloc(sizeof(buffer - 9)); //The reason its - 6 is because thats how many bytes "rmdir:" is.
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

    // convert version number into a string
    char version_str[10];
    snprintf(version_str, 10, "%d", version);

    // create a new path string with the version number folder
    char h_version_path[PATH_MAX];
    snprintf(h_version_path, PATH_MAX, "%s/%s", path, version_str);

    // access the dir

    // DIR *dir;
    // struct dirent *dp;
    // ino_t meta_inode;
    // if ((dir = opendir (h_version_path)) == NULL) {
    //     printf("Cannot open path %s...\n", h_version_path);
    //     return;
    // }
    // while ((dp = readdir(dir) ) != NULL){
    //     if (dp->d_type == DT_DIR){
    //         // this is a dir, so we need to loop through this as well
    //     }
    //     else{
    //         // this is a file, so send it off to the client
    //     }
    //     (void) closedir(dir);
    // }
    scanDir_sendFiles(h_version_path, sockfd);
    free(line1);
}

int scanDir_sendFiles(char *path, int sockfd){
    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir (path)) == NULL) {
        printf("Cannot open path %s...\n", path);
        return;
    }
    while ((dp = readdir(dir) ) != NULL){
        if (dp->d_type == DT_DIR){ // this is a dir, so we need to loop through this as well
            
            char newPath[PATH_MAX];
            snprintf(newPath, PATH_MAX, "%s/%s", path, dp->d_name);

            scanDir_sendFiles(newPath);
        }
        else{ // this is a file, so send it off to the client
            char newPath[PATH_MAX];
            snprintf(newPath, PATH_MAX, "%s/%s", path, dp->d_name);

            int fd = open(newPath, O_RDWR);
            struct threadData data = malloc(sizeof(struct threadData));
            data->fd = (int)malloc(4 * sizeof(int));
            memcpy(data->fd, fd, 4 * sizeof(int));

            data->path = (char *)malloc(strlen(newPath) * sizeof(char));
            memcpy(data->path,"\0", strlen(newPath) * sizeof(char)); // ensure it is a string
            strcpy(data->path, newPath);

            pthread_create(&thread_id_filePush, NULL, pushFileToClient, (void*)&data);
		    pthread_join(thread_id_filePush, NULL);
            // launch a thread and mutex it
        }
        (void) closedir(dir);
    }
}

void *pushFileToClient(void *dat){
    struct threadData data = (struct threadData)dat;
    struct stat fileStat;
    if(fstat(data->fd, &fileStat) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    char *buffer[fileStat->st_size];

    if(read(data->fd, buffer, fileStat->st_size) < 0){
        printf("There was an error reading file %s...\n", );
    }
    
    char *clientPath = getClientsPath(data->path);
    char *message;
    int len = strlen(clientPath) + strlen(buffer) + strlen("file:") + 1;
    // message
    snprintf(message, len, "file:%s:%s", clientPath, buffer);
    write(sockfd_local, message, len);
}

int getProjectCurrVersion(char *ProjectName){
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", ".repo", proj);
    char meta_path[PATH_MAX];
    snprintf(meta_path, PATH_MAX, "%s/%s", path, ".meta");
    int meta_fd = open(meta_path, O_RDONLY);
    if(meta_fd < 0) return;
    
    char *mata_buffer[10];
    char *version_str;
    int version;
    char *line1;
    if(read(meta_fd, mata_buffer, sizeof(mata_buffer)) == mata_buffer){
        line1 = getLine(mata_buffer); // this will be the version number
        version = atoi(line1);
        snprintf(version_str, strlen(line1), "%d", line1);
        close(meta_fd);
        return version;
    }
    else return -1;
}

char *getClientsPath(char *serverPath){
    char *temp = malloc(sizeof(char));
    memcpy(temp, "\0", sizeof(char));

    char *final = malloc(sizeof(char));
    memcpy(final, "\0", sizeof(char));
    char lett;
    int i;
    int side = 0;
    for(i=0; i<strlen(str); ++i){
        if(side = 0){
            lett = str[i];
            temp = charAppend(temp, lett);

        }
        if(lett == '/'){ // if the letter is a new line terminator then we exit the loop
            if(side == 0)
                side = 1;
            else
                side = 0;
        }
    }
    return temp;
}


char *getLine(char *str){
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