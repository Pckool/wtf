#ifndef SRV_H
#define SRV_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>

void *newUserThread(void *vargp);
int newUser(char* buffer);
void create(char* buffer);
void error(char *msg);

#endif
