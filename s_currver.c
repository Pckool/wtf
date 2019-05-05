#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include "h_global.h"

int directoryCounter_s(char* buffer){
        char *proj = malloc(sizeof(buffer - 8)); //The reason its - 8 is because thats how many bytes "currver:" is.
        int i = 0;
        int p = 8;
        int x = 0;
        DIR *dp;
        struct dirent *dir;
        while (i < sizeof(proj)){
                proj[i] = buffer[p]; //Transfer the project name from the buffer to proj.
                i++;
                p++;
        }
        char path[PATH_MAX]; //Pathway to the project directory
        snprintf(path, PATH_MAX, "%s/%s", ".repo", proj); //Formatting the pathway
        dp = opendir(path);
        while((dir = readdir(dp)) != NULL) { //The directory can be read
        	if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){ //The directory is not the "." or ".." directory
                	if(dir->d_type == DT_DIR){ //Makes sure it is a directory
                        	dircount++; //Increment directory count
                        }
		}
       	}
	return dircount;
}
