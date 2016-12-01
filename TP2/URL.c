#include "URL.h"

void initURL(URL *url) {
	memset(url->user, 0, SIZE);
	memset(url->pass, 0, SIZE);
	memset(url->host, 0, SIZE);
	memset(url->path, 0, SIZE);
}

int parseURL(URL *url, const char *urlString) {
	
}