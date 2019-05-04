#ifndef CLNT_H
#define CLNT_H

#define true 1
#define false 0

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
#include <openssl/sha.h>
#include <dirent.h>

// main
void error(char *msg);

void create(char* projectName);
void connecter(int fileSize);

// Configure
void configure(char* ip, char* port);
int getFile();

// Add/Remove
void add(char* proj, char* file);
char *replaceLine(char **content, char **line, char *newLine);
char *removeLine(char *content, char **line);
char *createaManLine(char *file, char *version, char *hash);

// Helping Function
int findDir(char *dirname);
char *stringAppend(char *str1, char *str2, int len);

#endif
