#ifndef URL_H
#define URL_H

#define PORT   21
#define SIZE   256
#define REGEXP "ftp://([([A-Za-z0-9])*:([A-Za-z0-9])*@])*([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+"

#include <string.h>
#include <stdlib.h>

typedef struct {
	char user[SIZE], pass[SIZE];
	char ip[SIZE], host[SIZE], path[SIZE];
} URL;

void initURL(URL *url);
int parseURL(URL *url, const char *urlString);

#endif