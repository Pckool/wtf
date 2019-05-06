#include "h_both.h"
#include "s_server.h"

void push_s(char *projectName){
    int dirStat = mkdir(projectName, S_IRWXU);
	if (!dirStat){ //If check passes
		printf("%s\n", "Project Created!");
	}
	DIR *dir;
	dir = opendir(projectName);
	char path[PATH_MAX];
}