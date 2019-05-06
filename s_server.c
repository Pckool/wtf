#include "h_both.h"
#include "s_server.h"

pthread_mutex_t mutexCreate = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexDestroy = PTHREAD_MUTEX_INITIALIZER;
int sockfd = -1;


void error(char *msg){
	perror(msg);
	exit(1);
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
	// snprintf(path, PATH_MAX, "%s/%s/%s",".repo", proj, ".Manifest");
	// int fd = open(path, O_RDWR | O_CREAT, 0600);
	// if(fd < 0){
	// 	printf("Failed to create .Manifest in server...\nError No: %d\n", fd);
	// }
	// close(fd);
	char* sendback[2];
}
int main(int argc, char* argv[])
{

	/*Most of this code except like two lines near the bottom is from the lecture 4/11/19
 	* server.c */

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

int newUser(char *buffer){
	// buffer now comes in as an address, it must be refrenced with *buffer to access the pointer
	//create a new thread
	printf("New User connected...\n");
	printf("recieved buffer: %s\n", buffer);
	pthread_t thread_id_destroy;
	pthread_t thread_id_create;
	pthread_t thread_id_push;
	pthread_t thread_id_checkout;

	if(startsWith(buffer, "checkout:")){
		pthread_create(&thread_id_checkout, NULL, newUserCheckoutThread, (void*) buffer);
		pthread_join(thread_id_checkout, NULL);
	}
	if(startsWith(buffer, "mkdir:")){
		pthread_create(&thread_id_create, NULL, newUserCreateThread, (void*) buffer);
		pthread_join(thread_id_create, NULL);
		// create_s(buffer);
		bzero(buffer,256);
	}
	else if(startsWith(buffer, "rmdir:")){ 
		pthread_create(&thread_id_destroy, NULL, newUserDestroyThread, (void*) buffer);
		pthread_join(thread_id_destroy, NULL);
		// pthread_mutex_lock(&mutexDestroy);

		// remove_directory_help(buffer);

		bzero(buffer,256);
		// pthread_mutex_unlock(&mutexDestroy);
	}
	

	if(true){
		return 0;
	}
}

void *newUserCreateThread(void *buffer){
	pthread_mutex_lock(&mutexCreate);
	printf("Created a new `create` thread for the user...\n");
	create_s((char *)buffer);
	pthread_mutex_unlock(&mutexCreate);
	return NULL;
}

void *newUserDestroyThread(void *buffer){
	pthread_mutex_lock(&mutexDestroy);
	printf("Created a new `destroy` thread for the user...\n");
	remove_directory_help((char *)buffer);
	pthread_mutex_unlock(&mutexDestroy);
	return NULL;
}

void *newUserCheckoutThread(void *buffer){
	printf("Created a new `checkout` thread for the user...\n");
	checkout_s((char *)buffer, sockfd);
	return NULL;
}
