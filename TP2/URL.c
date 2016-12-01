#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "URL.h"

URL *initURL() {
	URL *url = (URL *)malloc(sizeof(string));

	memset(url->path, 0, sizeof(string));
	memset(url->host, 0, sizeof(string));
	memset(url->user, 0, sizeof(string));
	memset(url->pass, 0, sizeof(string));

	return url;
}

int parseURL(URL *url, const char *urlString) {
	if (strncmp(urlString, "ftp://[", 7) == 0) {
		int i, size = 0;

		// Get username
		for (i = 7; i < strlen(urlString); i++) {
			if (urlString[i] != ':') {
				url->user[size++] = urlString[i];
			} else {
				size = 0;
				break;
			}
		}

		// Get password
		for (++i; i < strlen(urlString); i++) {
			if (urlString[i] != '@') {
				url->pass[size++] = urlString[i];
			} else {
				if (urlString[++i] != ']')
					return -1;

				url->pass[size] = '@';
				size = 0;
				break;
			}
		}

		// Get host
		for (++i; i < strlen(urlString); i++) {
			if (urlString[i] != '/') {
				url->host[size++] = urlString[i];
			} else {
				size = 0;
				break;
			}
		}

		// Get path
		for (++i; i < strlen(urlString); i++) {
			url->path[size++] = urlString[i];
		}
	} else {
		return -1;
	}

	return 0;
}