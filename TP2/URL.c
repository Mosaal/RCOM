#include <string.h>
#include <stdlib.h>
#include "URL.h"

void initURL(URL *url) {
	memset(url->path, 0, sizeof(string));
	memset(url->host, 0, sizeof(string));
	memset(url->user, 0, sizeof(string));
	memset(url->password, 0, sizeof(string));
	memset(url->fileName, 0, sizeof(string));
}

int parseURL(URL *url, const char *urlString) {
	// get user
	// get pass
	// get host
	// get path
	// get fileName
}