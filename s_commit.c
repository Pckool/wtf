#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>


void commit_s(char* buffer, int sockfd){
        char *proj = malloc(sizeof(buffer - 97));
        int i = 0;
        int p = 7;
        int x = 0;
        while (i < sizeof(proj)){
                proj[i] = buffer[p];
                i++;
                p++;
        }
        char* tok = strtok(proj, ":");
        tok = strtok(NULL, ":");
        int version = 0;
        DIR *dp;
        struct dirent *dir;
        char path[PATH_MAX]; //Pathway to the project directory
        snprintf(path, PATH_MAX, "%s/%s", ".repo",proj ); //Formatting the pathway
        dp = opendir(path);
        while((dir = readdir(dp)) != NULL) { //The directory can be read
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){ //The directory is not the "." or ".." directory
                        if(dir->d_type == DT_DIR && atoi(dir->d_name) > version){ //Makes sure it is a directory
                                bzero(path, PATH_MAX);
                                snprintf(path, PATH_MAX, "%s/%s/%s", ".repo", proj, dir->d_name);
                                version = atoi(dir->d_name);
                  }
                }
        }
        if (version == 0){
                bzero(path, PATH_MAX);
                snprintf(path, PATH_MAX, "%s/%s/%s", ".repo", proj,".Manifest");
        }
        else {
     		bzero(path, PATH_MAX);
                snprintf(path, PATH_MAX, "%s/%s/%s/%s", ".repo", proj, dir->d_name, ".Manifest");
        }
        int fd = open(path, O_RDONLY);
        char servman[5000];
        struct stat servbuff;
        read(fd, servman, 5000);
        write(sockfd, servman, 5000);
}
