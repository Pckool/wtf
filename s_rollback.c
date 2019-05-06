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

void delete(const char *path)
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;
   if (d) //Basically if the directory exists
   {
      struct dirent *p;
      r = 0;
      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;
          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }
          len = path_len + strlen(p->d_name) + 2; //IDK why but basically when messing with files and diretocries recursivley move by two is the best way to do it
          buf = malloc(len);

          if (buf)
          {
             struct stat statbuf;
             snprintf(buf, len, "%s/%s", path, p->d_name); //changes the path for the file
             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }
   if (!r)
   {
      r = rmdir(path);
   }
}


void rollback_s(char buffer[]){
	char *proj = malloc(sizeof(buffer - 9));
	int i = 0;
	int p = 9;
	int x = 0;
	while (i < sizeof(proj)){
		proj[i] = buffer[p];
		i++;
		p++;
	}
	char* tok = strtok(proj, ":");
	tok = strtok(NULL, ":");
	int version = atoi(tok);
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
				delete(path); 	
                  }
		}
       	}
	
}
