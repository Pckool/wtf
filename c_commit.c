#include "c_client.h"
#include "h_both.h" 


void commit_c(char* projectName){
        // 1: send a message top the server requesting the latest .Manifest
        // commit:<projectName>
        // 2: wait for the server to send the file back
        // 3: read the file into a buffer
        // 4: if the file is empty, skip the next few steps
        // 5: take .Manifest and rehash the files
        // 6: compare the server's .Manifest against the clients .Manifest located in the project folder
        // 7: If the .Manifest contains lines that start the same but are different hashes
                // append that line to the commit buffer with UMAD tag M
        // 8: if the .Manifest contains a line that is not in the server's .Manifest
                // append that to the commit buffer with UMAD tag A
        // 9: if the servers .manifest contains a line that .manifest does not
                // append that to the Commit buffer with the UMAD tad D
        // 10: create the .commit using the commit buffer
        // 11: send that to the server
        // DONE!

        // CONNECT TO THE SERVER
        struct stat *buf; //Needed to check the size of the config file
	buf = malloc(sizeof(struct stat));
	char* fp = ".configure";
        stat(fp, buf); //Gets stats about the file and puts it in the struct buf
        int size = buf->st_size;
        int sockfd = connecter(size); //Takes the size and goes to connecter
        
        // DONE
        char projectDir[PATH_MAX];

        DIR *dir;
        dir = opendir (projectName);
        if (dir == NULL) {
                if(findProject(projectName) < 0){
                        perror("ERROR Couldn't find the project directory, please go next to the project folder or inside of it.\n");
                        return;
                }
                else{
                        
                        if(getProjectDir(projectName) == NULL){
                                perror("ERROR Couldn't find the project directory, please go next to the project folder or inside of it.\n");
                                return;
                        }
                        snprintf(projectDir, PATH_MAX, "%s/%s", getProjectDir(projectName), ".Manifest"); //Path to manifest
                        printf("found %s\n", projectDir);
                }
    
        }
        closedir(dir);

        char commitPath[PATH_MAX];
        char manifestPath[PATH_MAX];

        char buffer[256] = "commit:"; //This is here because eventually we need to add the networking protocols
	int bufferStartLen = 7;
	int p = 0;
	while (p < strlen(projectName)){
		buffer[bufferStartLen] = projectName[p];
		bufferStartLen++;
		p++;
	}
        char *dataRecieved = sendAndRecieveMessage(buffer, sockfd);
        // this is for the data we just got
        DataLink *messageTokensHead = (DataLink *)malloc(sizeof(DataLink));
        messageTokensHead = newDataLink("_START_");

        messageTokensHead = tokenizeString(dataRecieved, ':', messageTokensHead);
        DataLink *messageTokens = messageTokensHead->next;
        
        int i = 0;
        

        if(strcmp(messageTokens->token, "file") == 0){
                // we have recieved a file stream header
                char *fileData;
                if(strcmp(messageTokens->next->token, projectName) == 0 ){
                        // we recieved a file in this project! Let's goooo
                        int yikes = atoi(messageTokens->next->next->token);
                        printf("%d num files\n", yikes);
                        while(i<yikes){
                                // we will loop through all of the file names given and wait for the server to send us it's data
                                fileData = RecieveMessage(sockfd);
                                printf("Message %s\n", fileData);
                                ++i;
                        }
                        //we have gotten the .Manifest from the server and stored it in fileData
                        // now we do the comparison
                        // Constructiong the directory to the .Manifest file
                        unsigned lenMan = strlen(projectDir) + strlen(".Manifest") + 1;
                        snprintf(manifestPath, lenMan+1, "%s/%s", projectDir, ".Manifest");

                        printf("Looking for %s\n", manifestPath);

                        // rehash function
                        // rehash(manifestPath);
                        // do a check to see if the rehash was a success

                        // tokenize both the .Manifest and the fileData based on \n
                        
                        DataLink *serverManifestHead = (DataLink *)malloc(sizeof(DataLink));
                        serverManifestHead = newDataLink("_START_");

                        serverManifestHead = tokenizeString(fileData, '\n', serverManifestHead);
                        DataLink *serverManifest = serverManifestHead->next;
                        DataLink *serverManifest_curr = serverManifest;

                        // tokenized the server's manifest
                         printf("tokenized the server's manifest \n");

                        int fd_man = open(manifestPath, O_RDWR);
                        if(fd_man < 0){
                                printf("ERROR trouble reading .Manifest from local path %s.\n", manifestPath);
                                return;
                        }
                        printf("opened .Manifest, aborting...\n");
                        struct stat fileStat_man;
                        if(fstat(fd_man, &fileStat_man) < 0){
                                printf("ERROR Could not get filedata for .Manifest, aborting...\n");
                                return;
                        }
                        printf("read the manifest...\n");
                        char manifest[fileStat_man.st_size];
                        read(fd_man, manifest, fileStat_man.st_size);
                        closedir(fd_man);

                        // re have read the .Manifest into var manifest

                        DataLink *clientManifestHead = (DataLink *)malloc(sizeof(DataLink));
                        clientManifestHead = newDataLink("_START_");

                        clientManifestHead = tokenizeString(manifest, '\n', clientManifestHead);
                        DataLink *clientManifest = clientManifestHead->next;
                        DataLink *clientManifest_curr = clientManifest;
                        // Tokenized the client's .manifest

                        // a buffer to hold the string to write to the .commit file
                        char *commitBuffer = (char *)malloc(2 * sizeof(char));
                        memcpy(commitBuffer, "\0", 2 * sizeof(char));
                        printf("made the commit buffer...\n");
                        // compairing time
                        while(clientManifest_curr != NULL){
                                
                                DataLink *clientManifestLineHead = (DataLink *)malloc(sizeof(DataLink));
                                        clientManifestLineHead = newDataLink("_START_");

                                        clientManifestLineHead = tokenizeString(manifest, '\n', clientManifestLineHead);
                                        DataLink *clientManifestLine = clientManifestLineHead->next;
                                        DataLink *clientManifestLine_curr = clientManifestLine;

                                while(serverManifest_curr != NULL){
                                        DataLink *serverManifestLineHead = (DataLink *)malloc(sizeof(DataLink));
                                        serverManifestLineHead = newDataLink("_START_");

                                        serverManifestLineHead = tokenizeString(fileData, '\n', serverManifestLineHead);
                                        DataLink *serverManifestLine = serverManifestLineHead->next;
                                        DataLink *serverManifestLine_curr = serverManifestLine;



                                        // Compare the first token of the n lines 
                                        if(strcmp(serverManifestLine_curr->token, clientManifestLine_curr->token) == 0){
                                                // the lines are the same
                                                // append M
                                                concat(commitBuffer, "M\t");
                                                while(clientManifestLine_curr!=NULL) {
                                                    concat(commitBuffer, clientManifestLine_curr->token);
                                                    if(clientManifestLine_curr->next != NULL) {
                                                            // if the next token in the line is NOT null (not at EOL)
                                                            concat(commitBuffer, "\t");
                                                    }   
                                                }
                                                concat(commitBuffer, "\n");
                                        }
                                        else{
                                                // the lines are different
                                                if(serverManifest_curr->next == NULL){
                                                        // if there is not next line in the server's manifest
                                                        // this means the client has a new file
                                                        // append with A
                                                        concat(commitBuffer, "A\t");
                                                        while(clientManifestLine_curr!=NULL){
                                                        concat(commitBuffer, clientManifestLine_curr->token);
                                                                if(clientManifestLine_curr->next != NULL){
                                                                        // if the next token in the line is NOT null (not at EOL)
                                                                        concat(commitBuffer, "\t");
                                                                }   
                                                        }
                                                        concat(commitBuffer, "\n");

                                                }
                                        }
                                        serverManifestLine_curr = serverManifestLine_curr->next;
                                        serverManifest_curr = serverManifest_curr->next;
                                }
                                clientManifestLine_curr = clientManifestLine_curr->next;
                                clientManifest_curr = clientManifest_curr->next;
                        }
                        // we are not using the UMAD notation, we are using UMA notation

                        if(strlen(commitBuffer) < 0){
                                printf("ERROR Something went wrong writing the commit buffer; aboring commmit...\n");
                                return;
                        }
                        unsigned lenComm = strlen(projectDir) + strlen(".Commit") + 1;
                        snprintf(commitPath, lenComm+1, "%s/%s", projectDir, ".Commit");
                        int fd_commit = open(commitPath, O_RDWR | O_CREAT);
                        if(fd_commit < 0){
                                printf("ERROR There was an issue creating the .commit file; aborting...\n");
                                return;
                        }
                        if(write(fd_commit, commitBuffer, strlen(commitBuffer)+1) < 0){
                                printf("ERROR There was an issue writing to the .commit file; aborting...\n");
                                return;
                        }


                }
        }

        
        
}


