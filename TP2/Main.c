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
	URL *url = (URL *)malloc(sizeof(URL));
	initURL(url);

	if (parseURL(url, argv[1]) == -1)
		return -1;

	// FTP
	return 0;
}