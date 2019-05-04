#include "c_client.h"

//char* ipAddr;
//char* portNo;
int sockfd = -1; //File Descriptor for socket


void error(char *msg){
	perror(msg);
	exit(1);
}

void create(char* projectName){
/* The reason why we have this code here to check the size of the configure file is because
 * when I had this in the connecter function it didn't work. I moved it here and it started to.
 */
	int n = -1;
	char message[256];
	struct stat *buf; //Needed to check the size of the config file
	buf = malloc(sizeof(struct stat));
	char* fp = ".configure";
        stat(fp, buf); //Gets stats about the file and puts it in the struct buf
        int size = buf->st_size;
	connecter(size); //Takes the size and goes to connecter
	char buffer[256] = "mkdir:"; //This is here because eventually we need to add the networking protocols
	int i = 6;
	int p = 0;
	while (p < strlen(projectName)){
		buffer[i] = projectName[p];
		i++;
		p++;
	}
	write(sockfd, buffer, strlen(buffer)); //Write the buffer that contains the name of the project and network protocol to the socket
	n = read(sockfd, message,255);
	printf("%s\n", message);
}

void connecter(int fileSize){
    int fd = open(".configure", O_RDONLY); //Opens configure file to read only
	if (fd == -1){
		error("Error. Configure file does not exist");
	}
    int portno;
    struct sockaddr_in serverAddressInfo;
    struct hostent *serverIPAddress;
    char buffer[256];
	read(fd, buffer, fileSize);
	int ipSize = 0; //Need to do alot of bullshit here to read the file and check its size to then be able to split it to IP address and port.
	int i = 0;
	while (buffer[i] != '\n'){
		ipSize++;
		i++;
	}
	char* ipAddress = (char *) malloc(ipSize - 1);
	i = 0;
	while (i < ipSize){
		ipAddress[i] = buffer[i];
		i++;
	}
	int portSize = fileSize - (ipSize + 2);
	i = 0;
	int t = ipSize + 1;
	char* portNum = (char *) malloc(portSize);
	while (i < portSize){
		portNum[i] = buffer[t];
		i++;
		t++;
	}
	int port = atoi(portNum); //Convert string to int
	serverIPAddress = gethostbyname(ipAddress); //Resolve IP from hostname
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char*)&serverAddressInfo, sizeof(serverAddressInfo)); //zero out. After this line I really dont get whats happening but it works
	serverAddressInfo.sin_family = AF_INET;
	serverAddressInfo.sin_port = htons(port);
	bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);
	
	if (connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0){
		error("ERROR connecting");
	}
}


int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 4){
		error("Error. Invalid number of inputs");
	}

	if (strcmp(argv[1], "configure") == 0){
		printf("%s\n"," Configuring the IP and port number");
		configure(argv[2], argv[3]);
	}

	if (strcmp(argv[1], "create") == 0){
		create(argv[2]);
	}
	if (strcmp(argv[1], "add") == 0){
        add(argv[2], argv[3]);
    }

}
