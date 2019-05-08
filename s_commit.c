#include "h_both.h"
#include "s_server.h"

pthread_t thread_id_filePush;

void commit_s(char* buffer, int sockfd){
        printf("Starting fetch routine with buffer %s...\n", buffer);
        char *proj = malloc(strlen(buffer - 7) + 1); //The reason its - 9 is because thats how many bytes "commit:" is.
        int i = 0;
        int p = 7;
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

        scanDir_sendManifest(h_version_path, sockfd, proj);

}

int scanDir_sendManifest(char *path, int sockfd, char *projectName){
        printf("Scanning to find the file(s) in `%s` to send to the user.\n", path);
        DIR *dir;
        struct dirent *dp;

        if ((dir = opendir (path)) == NULL) {
                printf("Cannot open path %s...\n", path);
                return -1;
        }
        char newPath[PATH_MAX];
        snprintf(newPath, PATH_MAX, "%s/%s", path, ".Manifest");
        int latest_manifest = open(newPath, O_RDONLY);
        if(latest_manifest < 0){
                printf("ERROR Something went wrong retrieving latest the .Manifest\n");
                return -1;
        }
        close(latest_manifest);
        threadData *data = (threadData *)malloc(sizeof(threadData));

        data->path = (char *)malloc(strlen(newPath) * sizeof(char) + 1);
        memcpy(data->path,"\0", strlen(newPath) * sizeof(char) + 1); // ensure it is a string
        strcpy(data->path, newPath);

        data->projectName = (char *)malloc(strlen(projectName) * sizeof(char) + 1);
        memcpy(data->projectName,"\0", strlen(projectName) * sizeof(char) + 1); // ensure it is a string
        strcpy(data->projectName, projectName);

        data->sockfd = sockfd;
        // send the FILE header messgae to client

        char *message;
        int len = strlen(projectName) + sizeof("1") + sizeof(".Manifest") + sizeof("file:")  ;
        
        message = (char *)malloc(len * sizeof(char));
        memcpy(message, "\0", len * sizeof(char));
        
        // char *byte_content = getByteContent(data->path);
        
        // snprintf(message, len, "file:%s:%s", data->projectName, byte_content);
        snprintf(message, len, "file:%s:%s:%s:%s", projectName, "1", ".Manifest");

        
        
        printf("This is sending to the client: %s\n", message);

        if(write(sockfd, message, len) < 0){
                printf("There was an issue writing to the socket...\n");
                return;
        }
        printf("Message sent successfully...\n");

        pthread_create(&thread_id_filePush, NULL, pushFileToClient, (void*)data);
        pthread_join(thread_id_filePush, NULL);
    
    
    return 0;
}

void *pushFileToClient(void *dat){
    printf("Sending file to client...\n");

    threadData *data = (threadData *)dat;


    int fd_data = open(data->path, O_RDWR);
    if(fd_data < 0){
        printf("there was a problem opening %s; aborting...\n", data->path);
        return;
    }

    struct stat fileStat;
    if(fstat(fd_data, &fileStat) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    char *project_buffer = (char *)malloc(fileStat.st_size);
    memcpy(project_buffer, "\0", fileStat.st_size);
    if(read(fd_data, project_buffer, fileStat.st_size) < 0){
        printf("There was an error reading file `%s`...\n", data->path);
        return;
    }
    if(fileStat.st_size == 0){
        project_buffer = realloc(project_buffer, 2);
        memcpy(project_buffer, "\0", 2);
        // project_buffer[0] = '_';
    }

    printf("\nRead data with %d bytes...\n\n", fileStat.st_size);
;
    
    // Sending file
    int amm = write(data->sockfd, project_buffer, fileStat.st_size);
    if(amm < 0){
        printf("There was an issue writing to the socket...\n");
        return;
    }
    printf("File sent successfully with %d/%d bytes written...\n", amm, fileStat.st_size);
    

    close(fd_data);
}