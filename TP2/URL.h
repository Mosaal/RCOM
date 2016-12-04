#ifndef URL_H
#define URL_H

#include "Utils.h"

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