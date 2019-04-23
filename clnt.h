#ifndef CLNT_H
#define CLNT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// main
void error(char *msg);
void configure(char* ip, char* port);
void create(char* projectName);
void connecter(int fileSize);

// Configure
void configure(char *ip, unsigned port);
int getFile();

#endif
