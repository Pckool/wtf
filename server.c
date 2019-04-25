#include "srv.h"

void error(char *msg){
	perror(msg);
	exit(1);
}

void create(char* buffer){
	char *proj = malloc(sizeof(buffer - 6)); //The reason its - 6 is because thats how many bytes "mkdir:" is.
	int i = 0;
	int p = 6;
	while (i < sizeof(proj)){
		proj[i] = buffer[p]; //Transfer the project name from the buffer to proj.
		i++;
		p++;
	}
	printf("%s\n", proj);
	int check = mkdir(proj, S_IRWXU); //tries to make the directory
	if(!check){ //If check passes
		printf("%s\n", "Directory Created!");
	}
}
int main(int argc, char* argv[])
{
/*Most of this code except like two lines near the bottom is from the lecture 4/11/19
 * server.c */
	int sockfd = -1;
	int newsockfd = -1;
	int portno = -1;
	int clilen = -1;
	int n = -1;
	char buffer[256];
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;
	if(argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		error("ERROR opening socket");
	}
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
	{
		error("setsockopt(SO_REUSEADDR) failed");
	}
	bzero((char *)&serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
	serverAddressInfo.sin_family = AF_INET;
	serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		error("ERROR on binding");
	}
	listen(sockfd, 0);
	clilen = sizeof(clientAddressInfo);
	newsockfd = accept(sockfd, (struct sockaddr *)&clientAddressInfo, &clilen);
	if (newsockfd < 0)
	{
		error("ERROR on accept");
	}

	// Main loop of the server. This will never exit until we recieve ctrl-c
	while(1){
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if(n < 0)
		{
			error("ERROR reading from socket");
		}
		else{
			int commStat; // the status of the command (if it was successful or not)
			printf("Here is the message: %s\n", buffer);
			n = write(newsockfd, "I got your message", 18);
			commStat = newUser(buffer); // will create a new thread and eventually will determine what the command the client is trying to use.

			if(n < 0)
			{
				error("ERROR writing to socket");
			}

			if(commStat < 0){
				printf("Something went wrong with the user's requested command...\n");
			}
		}


	}

	return 0;

}

int newUser(char* buffer){
	//create a new thread
	printf("New User connected...\n");
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, newUserThread, NULL);

	pthread_join(thread_id, NULL);

	if(true){
		return 0;
	}
}

void *newUserThread(void *vargp){
	printf("Created a new Thread...\n");
	create(buffer);
	return NULL;
}
