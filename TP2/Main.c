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
		printf("ERROR: Wrong number of arguments.\n");
		printUsage(argv[0]);
		return -1;
	}

	// URL
	URL url;
	initURL(&url);

	if (parseURL(&url, argv[1]) == -1) {
		printf("ERROR: Unable to parse URL.\n");
		printUsage(argv[0]);
		return -1;
	}

	if (getIpByHost(&url) == -1) {
		printUsage(argv[0]);
		return -1;
	}

	printURLInfo(url);
	printf("The IP received to %s was %s\n", url.host, url.ip);

	// FTP
	FTP ftp;
	initFTP(&ftp);

	return 0;
}