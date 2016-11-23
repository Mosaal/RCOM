#ifndef URL_H
#define URL_H

#define STRING_SIZE 256

typedef char string[STRING_SIZE];

typedef struct {
	string fileName;
	string ip, host, path;
	string user, password;
} URL;

URL *initURL();
int parseURL();

#endif