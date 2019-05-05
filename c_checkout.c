#include "c_client.h"
#include "h_both.h"

void checkout(char *projectName){
    // First we will check top see if the project file already exists on the client side
    // if it does not, then we can proceed

    // then we check to see if the server has the file
        // send a req to the server to let us know if the project exists
        // if it does, then we will recieve a 
	int n = -1;
	char message[256];
	struct stat *buf; //Needed to check the size of the config file
	buf = malloc(sizeof(struct stat));
	char* fp = ".configure";
        stat(fp, buf); //Gets stats about the file and puts it in the struct buf
        int size = buf->st_size;
	connecter(size); //Takes the size and goes to connecter

	char buffer[256] = "checkout:"; //This is here because eventually we need to add the networking protocols
	int bufferStartLen = 9;
	int p = 0;
	while (p < strlen(projectName)){
		buffer[bufferStartLen] = projectName[p];
		bufferStartLen++;
		p++;
	}
	write(sockfd, buffer, strlen(buffer)); //Write the buffer that contains the name of the project and network protocol to the socket
	n = read(sockfd, message,255);
	printf("%s\n", message);

	int dirStat = mkdir(projectName, S_IRWXU);
	if (!dirStat){ //If check passes
		printf("%s\n", "Project Created!");
	}
	DIR *dir;
	dir = opendir(projectName);
	char path[PATH_MAX];

	snprintf(path, PATH_MAX, "%s/%s", projectName, ".Manifest");
	int fd = open(path, O_RDWR | O_CREAT, 0600);
	if (fd < 0){
		printf("Failed to create .Manifest clientside...\nError No: %d\n", fd);
	}
	close(fd);
}