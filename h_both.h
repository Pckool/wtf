#ifndef BOTH_H
#define BOTH_H

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

#define MAX_FILE_SIZE 52428800  // this is the maximum file size we will allow. 50MB

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

#endif