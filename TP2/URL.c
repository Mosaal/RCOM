#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "URL.h"

URL *initURL() {
	URL *url = (URL *)malloc(sizeof(string));

	memset(url->path, 0, sizeof(string));
	memset(url->host, 0, sizeof(string));
	memset(url->user, 0, sizeof(string));
	memset(url->password, 0, sizeof(string));
	memset(url->fileName, 0, sizeof(string));

	return url;
}

int parseURL(URL *url, const char *urlString) {
	int i, size = 0, state = 0;

	for (i = 0; i < strlen(urlString); i++) {
		switch (state) {
			case 0:
				if (urlString[i] == 'f')
					state = 1;
				else
					state = -1;
				break;
			case 1:
				if (urlString[i] == 't')
					state = 2;
				else
					state = -1;
				break;
			case 2:
				if (urlString[i] == 'p')
					state = 3;
				else
					state = -1;
				break;
			case 3:
				if (urlString[i] == ':')
					state = 4;
				else
					state = -1;
				break;
			case 4:
				if (urlString[i] == '/')
					state = 5;
				else
					state = -1;
				break;
			case 5:
				if (urlString[i] == '/')
					state = 6;
				else
					state = -1;
				break;
			case 6:
				if (urlString[i] == '[')
					state = 7;
				else
					state = -1;
				break;
			case 7: // User
				if (urlString[i] == ':') {
					size = 0;
					state = 8;
				} else
					url->user[size++] = urlString[i];
				break;
			case 8: // Pass
				if (urlString[i] == '@') {
					url->password[size] = '@';
					size = 0;
					state = 9;
				} else
					url->password[size++] = urlString[i];
				break;
			case 9:
				if (urlString[i] == ']')
					state = 10;
				else
					state = -1;
				break;
			case 10: // Host
				if (urlString[i] == '/') {
					size = 0;
					state = 11;
				} else
					url->host[size++] = urlString[i];
				break;
			case 11: // Path
				url->path[size++] = urlString[i];
				break;
			case -1:
				return state;
		}
		// printf("state = %d\n", state);
	}

	return state;
}