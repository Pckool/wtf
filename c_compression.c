#include "c_client.h"
#include "h_both.h"


void sendTar(char *projectName){
    int sockfd = -1;
    FILE *fp;
    int ret,len;
    char buf[2000000] = {0};
    fp = fopen("/home/priyanka/3455.tgz","rb");
    while(len = fread(buf, 1, 2000000, fp))
    //while((buf[0] = fgetc(fp)) != EOF)
    {
        printf("\nRead : %d",len);
        ret = send(sockfd, buf, strlen(buf),0);
        printf("    Sent : %d",ret);
        if(ret == -1)
        {
            perror("Error sending data : Client");
        }
    }
    fclose(fp);
}