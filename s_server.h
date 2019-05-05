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
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>

void *newUserThread(void *vargp);
int newUser(char **buffer);
char *create_s(char *buffer);
void error(char *msg);

// checkout
void checkout_s(const char *buffer, int sockfd);

// Threads
void *newUserCreateThread(void *buffer);
void *newUserDestroyThread(void *buffer);
void *newUserCheckoutThread(void *buffer);


// Helping Function
int findDir(char *dirname);
char *stringAppend(const char *str1, const char *str2, int len);
char *concat(const char *s1, const char *s2);
int getLen(int x);
char* parseInt(const int num);
void removeSubstring(char *s,const char *toremove);
char *charAppend(char *str, char charr);

#endif
