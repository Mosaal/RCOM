#include <stdio.h>
#include <stdlib.h>
#include "FTP.h"
#include "URL.h"

void printUsage(char *command) {

}

int processArguments(char **argv) {

}

int main(int argc, char **argv) {
	FTP *ftp = initFTP();
	URL *url = initURL();

	printf("ftp: %p; %lu\n", ftp, sizeof(ftp));
	printf("url: %p; %lu\n", url, sizeof(url));

	free(ftp);
	free(url);

	return 0;
}