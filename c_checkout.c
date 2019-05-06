#include "c_client.h"
#include "h_both.h"

void checkout(char *projectName, int sockfd){
    // First we will check top see if the project file already exists on the client side
    // if it does not, then we can proceed

    // then we check to see if the server has the file
        // send a req to the server to let us know if the project exists
        // if it does, then we will recieve a 
	int n = -1; // the fd we will recieve 
	
	struct stat *buf; //Needed to check the size of the config file
	buf = malloc(sizeof(struct stat));
	char* fp = ".configure";
        stat(fp, buf); //Gets stats about the file and puts it in the struct buf
        int size = buf->st_size;
	int sockfd2 = connecter(size); //Takes the size and goes to connecter

	char buffer[256] = "checkout:"; //This is here because eventually we need to add the networking protocols
	int bufferStartLen = 9;
	int p = 0;
	while (p < strlen(projectName)){
		buffer[bufferStartLen] = projectName[p];
		bufferStartLen++;
		p++;
	}
	write(sockfd2, buffer, strlen(buffer)); //Write the buffer that contains the name of the project and network protocol to the socket
	
	int msg_length = 0;
	int waiting = true;

	// this will loop through until it recieves a readable responce from the socket.
	printf("waiting for server... |");
	fflush(stdout);

	while(waiting){
		loading();

		if(ioctl(sockfd2, FIONREAD, &msg_length) < 0){
			return;
		}
		if(msg_length == 0){
			//printf("%d\b\n", msg_length);
			continue;
		}
		else if(msg_length > 0){
			printf("The message size is %d...\n", msg_length);
			break;
		}
	}	

	// printf("\n%d is the size of the incomming data...\n", msg_length);

	char message[msg_length];
	n = read(sockfd2, message, msg_length);
	
	printf("%s with length %d\n", message, msg_length);
	///
	if(ioctl(sockfd2, FIONREAD, &msg_length) < 0){
		return;
	}
	printf("size of socket now: %d\n", msg_length);
	///
	DIR *dir;
	dir = opendir(projectName);
	if(dir == NULL){
		int status = prot_fileRecieve(message, msg_length, sockfd2);

		if(status < 0 ){
			// the 
			return;
		}
		system("tar -xzvf data.tar.gz");
	}
	else{
		printf("You already have a version downlaoded in this directory. Please move to another subdirectory or delete the project localy.\n");
	}
	closedir(dir);
	
	

		
	
	printf("Either we are done, or we gave up.\n");
	
}



tokenizeFileMsg *prot_tokenizeFileMsg(char *msgToTokenize, unsigned sizeOfMsg){
	tokenizeFileMsg *newTokens = (tokenizeFileMsg *)malloc(sizeof(tokenizeFileMsg));
	int i = 0;
	int part = 0;

	unsigned lenOfMsg = strlen(msgToTokenize);
	printf("This is the length of the string recieved: %d\n",lenOfMsg);
	char *msgCpy = (char *)malloc(lenOfMsg * sizeof(char));
	memcpy(msgCpy, "\0", lenOfMsg);
	strcpy(msgCpy, msgToTokenize);

	char *projectName = "";
	char *data = "";


	while( i < lenOfMsg){
		
		switch(part){
			case 0:
				printf("Case 0\n");
				removeSubstring(msgCpy, "file:");
				++part;
				
				break;
			case 1:
				printf("Case 1\n");
				if(msgCpy[i] != ':'){
					charAppend(projectName, msgCpy[i]);
				}
				else{
					newTokens->projectName = (char *)malloc(strlen(projectName));
					memcpy(newTokens->projectName, projectName, strlen(projectName));
					++part;
				}
				++i;
				break;
			case 2:
				if(msgCpy[i] != ':'){
					charAppend(data, msgCpy[i]);
				}
				++i;
				if(i == lenOfMsg){
					newTokens->data = (char *)malloc(strlen(data));
					memcpy(newTokens->data, data, strlen(data));
					printf("assigned data...\n");
					++part;
				}
				
				break;
			default:
				++i;
				break;
		}
	}
	return newTokens;
	
}