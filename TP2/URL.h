#ifndef URL_H
#define URL_H

#define SIZE   256
#define REGANO "ftp://([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+"
#define REGEXP "ftp://([([A-Za-z0-9])*:([A-Za-z0-9])*@])*([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+"

#include <stdio.h>
#include <netdb.h>
#include <regex.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
	char ip[SIZE], host[SIZE];
	char user[SIZE], pass[SIZE];
	char path[SIZE], file[SIZE];
} URL;

void initURL(URL *url);

int correctFormat(const char *urlString);
int parseURL(URL *url, const char *urlString);
void getStringUntilChar(const char *urlString, char *string, char c, int *index);

int getIpByHost(URL *url);
void reverseString(char *str);

#endif
