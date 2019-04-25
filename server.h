#ifndef SRV_H
#define SRV_H

#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h> 

void *newUserThread(void *vargp);
int newUser(char* buffer);
void create(char* buffer);
void error(char *msg);

#endif