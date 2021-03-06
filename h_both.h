#ifndef BOTH_H
#define BOTH_H

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
#include <sys/ioctl.h>
#include <sys/sendfile.h>

#define MAX_FILE_SIZE 52428800  // this is the maximum file size we will allow. 50MB
typedef struct ProtocolLink{
	char *token;
	struct ProtocolLink *next;
}ProtocolLink;

typedef struct DataLink{
	char *token;
	struct DataLink *next;
}DataLink;

// Helping Function
int findDir(char *dirname);
char *stringAppend(const char *str1, const char *str2, int len);
char *concat(const char *s1, const char *s2);
int getLen(int x);
char* parseInt(const int num);
void removeSubstring(char *s, const char *toremove);
char *charAppend(char *str, char charr);
char *getLine(char *str);
void loading();
char *getByteContent(char *filePath);
//tokenizing
DataLink *tokenizeString(char *inputMsg, char separator, DataLink *head);
DataLink *newDataLink(char *token);
void printDataLinks(DataLink *head);
ProtocolLink *tokenizeProtocolMessage(char *inputMsg, ProtocolLink *head);
ProtocolLink *newProtocolLink(char *token);
void printTokenLinks(ProtocolLink *head);
int prot_fileRecieve(char *message, const unsigned msg_length, int sockfd);

char *sendAndRecieveMessage(char *buffer, int sockfd);
char *RecieveMessage(int sockfd);

int waitForSocketMessage(int sockfd);

#endif