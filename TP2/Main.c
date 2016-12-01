#include <stdio.h>
#include "URL.h"
#include "FTP.h"

void printUsage(char *argv0) {
	printf("Usage: %s ftp://[<user>:<pass>@]<host>/<url-path>\n", argv0);
}

void printURLInfo(URL url) {
	printf("User: %s\n", url.user);
	printf("Pass: %s\n", url.pass);
	printf("Host: %s\n", url.host);
	printf("Path: %s\n", url.path);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("WARNING: Wrong number of arguments.\n");
		printUsage(argv[0]);
		return -1;
	}

	// Process URL
	URL url;
	initURL(&url);

	if (parseURL(&url, argv[1]) == -1) {
		printf("ERROR: Unable to parse URL.\n");
		printUsage(argv[0]);
		return -1;
	}

	printURLInfo(url);

	// Process FTP

	return 0;
}