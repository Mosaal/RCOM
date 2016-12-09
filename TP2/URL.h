#ifndef URL_H
#define URL_H

#include "Utils.h"

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