#ifndef URL_H
#define URL_H

#define PORT   21
#define SIZE   256
#define REGEXP "ftp://([([A-Za-z0-9])*:([A-Za-z0-9])*@])*([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+"

#include <regex.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
	char user[SIZE], pass[SIZE];
	char ip[SIZE], host[SIZE], path[SIZE];
} URL;

void initURL(URL *url);
int parseURL(URL *url, const char *urlString);
int correctFormat(const char *urlString);
void getStringUntilChar(const char *urlString, char *string, char c, int *index);
int getIpByHost(URL *url);

#endif