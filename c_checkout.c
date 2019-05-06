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

	printf("waiting for server...\n");
	// this will loop through until it recieves a readable responce from the socket.
	while( msg_length != 0){
		printf(".");
		if(ioctl(sockfd2, FIONREAD, &msg_length) < 0){
			printf("No data recieved from the server");
			continue;
		}
		printf("\n");

		char message[msg_length];
		n = read(sockfd2, message, msg_length);
		printf("%s with length %d\n", message, msg_length);

		if(startsWith(message, "file:")){
			// this means we are recieving the correct message...
			tokenizeFileMsg *tokenizedData = prot_tokenizeFileMsg(message);

			int data_len = strlen(tokenizedData->data) - 15;
			// char data_msg[data_len];
			// int k = 0;
			// int data_counter = msg_length - data_len;
			// for(k = 0; k<reason_len; k++){
			// 	data_msg[k] = message[data_counter];
			// 	++data_counter;
			// }
			// int dirStat = mkdir(projectName, S_IRWXU);
			// if (!dirStat){ //If check passes
			// 	printf("%s\n", "Project Created!");
			// }

			DIR *dir;
			dir = opendir(projectName);
			closedir(dir);
			//char path[PATH_MAX];

			//snprintf(path, PATH_MAX, "%s/%s", projectName, "data.tar.gz");
			int fd = open("./data.tar.gz", O_RDWR | O_CREAT, 0600);
			if (fd < 0){
				printf("Failed to create compressed data clientside...\nError No: %d\n", fd);
			}
			if(write(fd, tokenizedData->data, data_len) < 0){
				printf("There was a problem writing compressed data to local dir.\n");
			}
			system("tar -xzvf data.tar.gz");
			close(fd);

		}
		else if(startsWith(message, "checkout:fail:")){
			// The project doesn't exist
			int reason_len = msg_length - 14;
			char reason_msg[reason_len];
			int k = 0;
			int msg_counter = msg_length - reason_len;
			for(k = 0; k<reason_len; k++){
				reason_msg[k] = message[msg_counter];
				++msg_counter;
			}
			printf("Checkout Failed...\n\tReason: %s\n", reason_msg);
		}

		
	}
	
	
}

// a protocol function to accept a file string sent over the network and tokenize it
tokenizeFileMsg *prot_tokenizeFileMsg(char *msgToTokenize){
	tokenizeFileMsg *newTokens = (tokenizeFileMsg *)malloc(sizeof(tokenizeFileMsg));
	int i = 0;
	int part = 0;
	int len = strlen(msgToTokenize);
	char *msgCpy = (char *)malloc(len * sizeof(char));
	memcpy(msgCpy, "\0", len);
	strcpy(msgCpy, msgToTokenize);

	char *projectName = "";
	char *data = "";

	unsigned lenOfMsg = strlen(msgToTokenize);

	while(i < lenOfMsg){
		switch(part){
			case 0:
				removeSubstring(msgCpy, "file:");
				++part;
				break;
			case 1:
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
				else{
					newTokens->data = (char *)malloc(strlen(data));
					memcpy(newTokens->data, data, strlen(data));
					++part;
				}
				++i;
				break;
		}
	}
	return newTokens;
	
}