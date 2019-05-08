#ifndef CLNT_H
#define CLNT_H

#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <dirent.h>

typedef struct manLineTokens{
	char *path;
	char *version;
	char *hash;
}manLineTokens;

typedef struct tokenizeFileMsg{
	char *projectName;
	char *data;
}tokenizeFileMsg;

// int sockfd = -1; //File Descriptor for socket

// main
void error(char *msg);

void create(char* projectName);
int connecter(int fileSize);

// Configure
void configure(char* ip, char* port);
int getFile();

// Add/Remove
void add(char* proj, char* file);
char *replaceLine(char **content, char **line, char *newLine);
int removeLine(char **content, char **line);
void c_remove(char *proj, char *file);
char *createaManLine(char *file, char *version, char *hash);
manLineTokens *tokenizeLine(char *line);
char *readLine(char *str);
//  push
void push_c(char *projectName);
char *getProjectDir(char *projectName);

// commit
void commit_c(char* projectName, int sockfd);



// Checkout
void checkout(char *projectName, int sockfd);
tokenizeFileMsg *prot_tokenizeFileMsg(char *msgToTokenize, unsigned sizeOfMsg);

// Helping Function
int findDir(char *dirname);
char *stringAppend(const char *str1, const char *str2, int len);
char *concat(const char *s1, const char *s2);
int getLen(int x);
char* parseInt(const int num);
void removeSubstring(char *s,const char *toremove);
char *charAppend(char *str, char charr);

#endif
