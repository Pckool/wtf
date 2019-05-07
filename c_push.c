#include "c_client.h"
#include "h_both.h"

void push_c(char *projectName){
    // get data from .Commit 
    // find the file paths listed
    // if they are all valid

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

    DataLink *filesToSend = (DataLink *)malloc(sizeof(DataLink));
    filesToSend = newDataLink("_START_");

    DataLink *currFile = (DataLink *)malloc(sizeof(DataLink));
    currFile = filesToSend;
    int i = 0;
    int countAU = 0;

    while(i <strlen(comm_buffer)){ // loop to go through each line
        char *line = getLine(comm_buffer);
        tokenizeString(line, '\t', lineData);
        if(lineData->next == NULL){ // This is to check if it is a version number (at the top of the file)
            lineData = lineData+strlen(line);
            i = i + strlen(line);
            continue;
        }
        // if it is not the version number
        
        if(strcmp(lineData->next->token, "U") == 0 || strcmp(lineData->next->token, "A") == 0){ // if the line is U or M or A
            int fd = open(lineData->token, O_RDONLY); // file listed in the .Commit

            struct stat fileStat_file;
            if(fstat(fd, &fileStat_file) < 0){
                printf("Could not get filedata, aborting...\n");
                return;
            }
            char file_buffer[fileStat_file.st_size];
            if(read(fd, comm_buffer, fileStat_file.st_size) < 0){
                printf("There was an error reading file `%s`...\n", lineData->token);
            }
            // ew have now read the file, get ready to save it into a data linked list
            currFile->next = newDataLink(lineData->token);
            currFile = currFile->next;
            i = i + strlen(line);
            lineData = lineData+strlen(line);
            ++countAU;
        }
        

        free(lineData);
    }
    // we have gone through the .Commit file and found all of the U and A lines and added them to a datastructure called filesToSend
    char *pushMessage = (char *)malloc(sizeof("push") * sizeof(char));
    memcpy(pushMessage, "\0", sizeof("push") * sizeof(char));
    strcpy(pushMessage, "push");

    char *filesToTar = (char *)malloc(1 * sizeof(char));
    memcpy(filesToTar, "\0", 1 * sizeof(char));
    strcpy(filesToTar, "");
    

    currFile = filesToSend;
    while(currFile != NULL){ // loop through the struct and append
        int orgSize = strlen(filesToTar); 
        realloc(filesToTar, strlen(filesToTar) + strlen(currFile->token) + 2);
        memcpy(pushMessage+orgSize, " ", 1);
        memcpy(filesToTar+orgSize+1, currFile->token, strlen(currFile->token) + 1);
        currFile = currFile->next;
    }
    
    int lenOfBuff =  strlen(filesToTar) + strlen("tar -czvf test.tar.gz ") + 1;
    char *tarCommand = (char *)malloc(lenOfBuff);
    memcpy(tarCommand, "\0", lenOfBuff);
    snprintf(tarCommand, lenOfBuff, "tar -czvf test.tar.gz %s", filesToTar);

    system(tarCommand);
    // this will send the tar command
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
            printf("Found DIR path: %s\n", temp);
            return temp;
        }
        else
            currPathPart = currPathPart->next;
    }
}