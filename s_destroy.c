// #include <stdio.h>
// #include <stdlib.h>
// #include <strings.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <dirent.h>
// #include <fcntl.h>
// #include "h_both.h"
#include "s_server.h"

remove_directory_help(const char *buffer){
 	char *proj = malloc(sizeof(buffer - 6)); //The reason its - 6 is because thats how many bytes "rmdir:" is.
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
	remove_directory(path);
}


int remove_directory(const char *path)
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
   return r;
}
