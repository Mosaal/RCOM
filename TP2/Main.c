#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FTP.h"
#include "URL.h"

void printUsage(char *argv0) {
	printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv0);
}

void printInfo(URL *url) {
	printf("Path: %s\n", url->path);
	printf("Host: %s\n", url->host);
	printf("User: %s\n", url->user);
	printf("Pass: %s\n", url->pass);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("WARNING: Wrong number of arguments!\n");
		printUsage(argv[0]);
		return -1;
	}

	// URL
	URL *url = initURL();

	if (parseURL(url, argv[1]) == -1) {
		printf("ERROR: Unable to parse the URL.\n");
		printUsage(argv[0]);
		return -1;
	}

	printInfo(url);

	// FTP
	return 0;
}