#include <stdio.h>
#include <stdlib.h>
#include "FTP.h"
#include "URL.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("WARNING: Wrong number of arguments!\n");
		printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
		return -1;
	}

	// URL
	URL *url = initURL();

	if (parseURL(url, argv[1]) == -1) {
		printf("ERROR: Unable to parse the URL.\n");
		printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
		return -1;
	}

	printf("Path: %s\n", url->path);
	printf("Host: %s\n", url->host);
	printf("User: %s\n", url->user);
	printf("Password: %s\n", url->password);
	printf("Filename: %s\n", url->fileName);

	// FTP
	return 0;
}