#include "URL.h"

void initURL(URL *url) {
	memset(url->ip, 0, SIZE);
	memset(url->user, 0, SIZE);
	memset(url->pass, 0, SIZE);
	memset(url->host, 0, SIZE);
	memset(url->path, 0, SIZE);
	memset(url->file, 0, SIZE);
}

int parseURL(URL *url, const char *urlString) {
	if (correctFormat(urlString) == 0) {
		int i, size = 0, index = 7;

		getStringUntilChar(urlString, url->user, ':', &index);
		getStringUntilChar(urlString, url->pass, ']', &index);
		getStringUntilChar(urlString, url->host, '/', &index);
		getStringUntilChar(urlString, url->path, '\0', &index);

		// retrieve file name
		for (i = strlen(url->path) - 1; i >= 0; i--) {
			if (url->path[i] != '/') {
				url->file[size++] = url->path[i];
				url->path[i] = 0;
			} else {
				reverseString(url->file);
				break;
			}
		}

		return 0;
	}

	return -1;
}

int correctFormat(const char *urlString) {
	char *tempURL;
	regmatch_t pmatch[strlen(urlString)];
	regex_t *regex = (regex_t *)malloc(sizeof(regex_t));

	tempURL = (char *)malloc(strlen(urlString));
	strcpy(tempURL, urlString);

	int reti;
	if ((reti = regcomp(regex, REGEXP, REG_EXTENDED)) != 0) {
		printf("ERROR: URL format is wrong.\n");
		return -1;
	}

	if ((reti = regexec(regex, tempURL, strlen(tempURL), pmatch, REG_EXTENDED)) != 0) {
		printf("ERROR: URL could not execute.\n");
		return -1;
	}

	free(regex);
	free(tempURL);

	return 0;
}

void getStringUntilChar(const char *urlString, char *string, char c, int *index) {
	int size = 0;
	for (; *index < strlen(urlString); (*index)++) {
		if (urlString[*index] != c) {
			string[size++] = urlString[*index];
		} else {
			(*index)++;
			break;
		}
	}
}

int getIpByHost(URL *url) {
	struct hostent *h;

	if ((h = gethostbyname(url->host)) == NULL) {
		printf("ERROR: Failed to get IP address.\n");
		return -1;
	}

	char *ip = inet_ntoa(*((struct in_addr *) h->h_addr));
	strcpy(url->ip, ip);

	return 0;
}

void reverseString(char *str) {
	char temp;
	int i = 0, j = strlen(str) - 1;

	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}