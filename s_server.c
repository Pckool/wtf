#include "s_server.h"


void error(char *msg){
	perror(msg);
	exit(1);
}

void destroy_s(char* buffer){
	 char *proj = malloc(sizeof(buffer - 6)); //The reason its - 6 is because thats how many bytes "mkdir:" is.
        int i = 0;
        int p = 6;
        int x = 0;
        while (i < sizeof(proj)){
                proj[i] = buffer[p]; //Transfer the project name from the buffer to proj.
                i++;
                p++;
        }
	char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", ".repo", proj);
        int check = rmdir(path, S_IRWXU); //tries to make the directory
        bzero(path, PATH_MAX);
        if(!check){ //If check passes
                printf("%s\n", "Directory Destroyed!");
        }

} 
char* create_s(char* buffer){
	char *proj = malloc(sizeof(buffer - 6)); //The reason its - 6 is because thats how many bytes "mkdir:" is.
	int i = 0;
	int p = 6;
	int x = 0;
	while (i < sizeof(proj)){
		proj[i] = buffer[p]; //Transfer the project name from the buffer to proj.
		i++;
		p++;
	}
	mkdir(".repo", S_IRWXU); //Makes a .repo directory where the server will store all the projects.
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/%s", ".repo", proj);
	int check = mkdir(path, S_IRWXU); //tries to make the directory
	bzero(path, PATH_MAX);
	if(!check){ //If check passes
		printf("%s\n", "Directory Created!");
	}
	DIR *dir;
	dir = opendir(proj);
	snprintf(path, PATH_MAX, "%s/%s/%s",".repo", proj, ".Manifest");
	int fd = open(path, O_CREAT, 0600);
	char* sendback[2];
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
	int k = 1;
	char buffer[256];
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;
	if(argc < 2){
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
	printf("Server Started...\n");
	// Main loop of the server. This will never exit until we recieve ctrl-c
	while(k == true){
		

		listen(sockfd, 0);
		clilen = sizeof(clientAddressInfo);
		newsockfd = accept(sockfd, (struct sockaddr *)&clientAddressInfo, &clilen);
		if (newsockfd < 0)
		{
			error("ERROR on accept");
		}
		
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if(n < 0)
		{
			error("ERROR reading from socket");
		}
		else{
			int commStat; // the status of the command (if it was successful or not)
			commStat = newUser(buffer); // will create a new thread and eventually will determine what the command the client is trying to use.
			if(strncmp(buffer, "mkdir:", 6)){
				create_s(buffer);
			}
			if(strncmp(buffer, "rmdir:", 6)){
                                destroy_s(buffer);
                        }
 
			n = write(newsockfd, buffer, 255);
			bzero(buffer, 255);
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
	printf("recieved buffer: %s\n", buffer);
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, newUserThread, (void*) &buffer);
	pthread_join(thread_id, NULL);

	if(true){
		return 0;
	}
}

void *newUserThread(void *buffer){
	printf("Created a new Thread...\n");
	create_s((char *)buffer);
	return NULL;
}
