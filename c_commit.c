#include "c_client.h"
#include "h_both.h" 


void commit_c(char* projectName, int sockfd){
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

        if(findProject(projectName) < 0){
                printf("ERROR You are not in the project directory of %s; aborting...\n", getProjectDir);
                return;
        }
        char *projectDir = getProjectDir(projectName);
        char commitPath[PATH_MAX];
        char manifestPath[PATH_MAX];

        if(projectDir == NULL){
                printf("ERROR Couldn't form the path to the project %s; aborting...\n", getProjectDir);
        }
        char buffer[256] = "commit:"; //This is here because eventually we need to add the networking protocols
	int bufferStartLen = 7;
	int p = 0;
	while (p < strlen(projectName)){
		buffer[bufferStartLen] = projectName[p];
		bufferStartLen++;
		p++;
	}
        char *dataRecieved = sendAndRecieveMassage(buffer, sockfd);
        // this is for the data we just got
        Datalink *messageTokensHead = (Datalink *)malloc(sizeof(Datalink));
        messageTokensHead = newDataLink("_START_");

        Datalink *messageTokensHead = tokenizeString(dataRecieved, ':', messageTokens);
        Datalink *messageTokens = messageTokensHead->next;
        
        int i = 0;
        
        if(strcmp(messageTokensHead->token, "file") == 0){
                // we have recieved a file stream header
                char *fileData;
                if(strcmp(messageTokensHead->next->token, projectName) == 0 ){
                        // we recieved a file in this project! Let's goooo
                        while(i<atoi(messageTokensHead->next->next->token){
                                // we will loop through all of the file names given and wait for the server to send us it's data
                                fileData = RecieveMessage(int sockfd);
                                ++i;
                        }
                        //we have gotten the .Manifest from the server and stored it in fileData
                        // now we do the comparison
                        // Constructiong the directory to the .Manifest file
                        unsigned lenMan = strlen(projectDir) + strlen(".Manifest") + 1;
                        snprintf(manifestPath, lenMan+1, "%s/%s", projectDir, ".Manifest");

                        // rehash function
                        // rehash(manifestPath);
                        // do a check to see if the rehash was a success

                        // tokenize both the .Manifest and the fileData based on \n
                        
                        Datalink *serverManifestHead = (Datalink *)malloc(sizeof(Datalink));
                        serverManifestHead = newDataLink("_START_");

                        Datalink *serverManifestHead = tokenizeString(fileData, '\n', messageTokens);
                        Datalink *serverManifest = serverManifestHead->next;
                        Datalink *serverManifest_curr = serverManifest;

                        // tokenized the server's manifest
                        
                        int fd_man = open(manifestPath, O_RDWR);
                        if(fd_man < 0){
                                printf("error reading .Manifest from local path %s.\n", manifestPath);
                        }
                        struct stat fileStat_man;
                        if(fstat(fd_man, &fileStat_man) < 0){
                                printf("Could not get filedata for .Manifest, aborting...\n");
                                return;
                        }
                        char manifest[fileStat_man.st_size];
                        read(fd_man, manifest, fileStat_man.st_size);
                        closedir(fd_man);

                        // re have read the .Manifest into var manifest

                        Datalink *clientManifestHead = (Datalink *)malloc(sizeof(Datalink));
                        clientManifestHead = newDataLink("_START_");

                        Datalink *clientManifestHead = tokenizeString(manifest, '\n', messageTokens);
                        Datalink *clientManifest = clientManifestHead->next;
                        Datalink *clientManifest_curr = clientManifest;
                        // Tokenized the client's .manifest

                        // a buffer to hold the string to write to the .commit file
                        char *commitBuffer = (char *)malloc(2 * sizeof(char));
                        memcpy(commitBuffer, "\0", 2 * sizeof(char));
                        // compairing time
                        while(clientManifest_curr != NULL){
                                
                                Datalink *clientManifestLineHead = (Datalink *)malloc(sizeof(Datalink));
                                        clientManifestLineHead = newDataLink("_START_");

                                        Datalink *clientManifestLineHead = tokenizeString(manifest, '\n', messageTokens);
                                        Datalink *clientManifestLine = clientManifestLineHead->next;
                                        Datalink *clientManifestLine_curr = clientManifestLine;

                                while(servertManifest_curr != NULL){
                                        Datalink *serverManifestLineHead = (Datalink *)malloc(sizeof(Datalink));
                                        serverManifestLineHead = newDataLink("_START_");

                                        Datalink *serverManifestLineHead = tokenizeString(fileData, '\n', messageTokens);
                                        Datalink *serverManifestLine = serverManifestLineHead->next;
                                        Datalink *serverManifestLine_curr = serverManifestLine;



                                        // Compare the first token of the n lines 
                                        if(strcmp(serverManifestLine_curr->token, clientManifestLine_curr->token) == 0){
                                                // the lines are the same
                                                // append M
                                                concat(commitBuffer, "M\t");
                                                while(clientManifestLine_curr!=NULL){
                                                    concat(commitBuffer, clientManifestLine_curr->token);
                                                    if(clientManifestLine_curr->NULL != NULL){
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
                                                                if(clientManifestLine_curr->NULL != NULL){
                                                                        // if the next token in the line is NOT null (not at EOL)
                                                                        concat(commitBuffer, "\t");
                                                                }   
                                                        }
                                                        concat(commitBuffer, "\n");

                                                }
                                        }
                                        serverManifestLine_curr = serverManifestLine_curr->next;
                                        servertManifest_curr = servertManifest_curr->next;
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
                        int fd_commit = open(commitPath, O_RDWR | O_CREATE);
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


