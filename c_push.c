#include "c_client.h"
#include "h_both.h"

void push_c(char *projectName){
    // get data from .Commit 
    // find the file paths listed
    // if they are all valid
        // update the version number in the .manifest
        // system call tar to compress the individual files into one tar file
        // send the new data.tar.gz and the .manifest over the network
        // delete the tar file
    if(findProject(projectName) < 0){
        printf("ERROR You are not in the project directory of %s; aborting...\n", getProjectDir);
        return;
    }
    char *projectDir = getProjectDir(projectName);
    if(projectDir == NULL){
        printf("ERROR Couldn't form the path to the project %s; aborting...\n", getProjectDir);
    }
    char *commitPath = "";
    unsigned lenComm = strlen(projectDir) + strlen(".Commit") + 1;
    snprintf(commitPath, lenComm, "%s/%s", projectDir, ".Commit");

    int fd_comm = open(commitPath, O_RDWR);
    if(fd_comm < 0){
        printf("ERROR There was a problem opening .Commit. Did you commit already?");
    }
    struct stat fileStat_comm;
    if(fstat(fd_comm, &fileStat_comm) < 0){
        printf("Could not get filedata, aborting...\n");
        return;
    }
    // reading .commit
    char comm_buffer[fileStat_comm.st_size];
    if(read(fd_comm, comm_buffer, fileStat_comm.st_size) < 0){
        printf("There was an error reading file `%s`...\n", commitPath);
    }

    // We now have a buffer called comm_buffer that contains the data from .Commit
    // let's tokenize this
    DataLink *lineData = (DataLink *)malloc(sizeof(DataLink));
    lineData = newDataLink("_START_");
    int i = 0;
    while(i <strlen(comm_buffer)){
        char *line = getLine(comm_buffer);
        tokenizeString(line, '\t', lineData);
        if(lineData->next == NULL) // This is to check if it is a version number (at the top of the file)
            return;
        while(false){

        }
    }
    
}

char *getProjectDir(char *projectName){
    DataLink *pathParts = (DataLink *)malloc(sizeof(DataLink));
    pathParts = newDataLink("_START_");
    tokenizeString(projectName, '/', pathParts);
    char *temp = (char *)malloc(1);
    memcpy(temp, "\0", 1); // literally an empty string

    DataLink *currPathPart;

    while(true){
        if(currPathPart == NULL)
            return NULL;

        concat(temp, currPathPart->token);

        if(strcmp(currPathPart->token, projectName) == 0){
            return temp;
        }
        else
            currPathPart = currPathPart->next;
    }
}