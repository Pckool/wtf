#include "s_server.h"
#include "h_both.h"

void checkout_s(const char *buffer, int sockfd){
    char *proj = malloc(sizeof(buffer - 9)); //The reason its - 6 is because thats how many bytes "rmdir:" is.
    int i = 0;
    int p = 9;
    int x = 0;
    while (i < sizeof(proj)){
        proj[i] = buffer[p]; //Transfer the project name from the buffer to proj.
        i++;
        p++;
    }
	char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", ".repo", proj);
}