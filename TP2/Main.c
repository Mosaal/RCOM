#include "URL.h"
#include "FTP.h"

void printUsage(char *argv0) {
	printf("Usage: %s ftp://[<user>:<pass>@]<host>/<url-path>\n", argv0);
}

void printURLInfo(URL url) {
	printf("The info retrieved from the URL was:\n");
	printf(" - User: %s\n", url.user);
	printf(" - Pass: %s\n", url.pass);
	printf(" - Host: %s\n", url.host);
	printf(" - Path: %s\n", url.path);
	printf(" - File: %s\n", url.file);
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
	printf("The IP received to '%s' was %s.\n\n", url.host, url.ip);

	// FTP
	FTP ftp;
	initFTP(&ftp, url.ip);

	if (ftpLogin(&ftp, url.user, url.pass) == -1) {
		printf("ERROR: Failed to login.\n");
		printUsage(argv[0]);
		return -1;
	}

	if (ftpCWD(&ftp, url.path) == -1) {
		printf("ERROR: Failed to change working directory.\n");
		printUsage(argv[0]);
		return -1;
	}

	if (ftpPasv(&ftp) == -1) {
		printf("ERROR: Failed to enter passive mode.\n");
		printUsage(argv[0]);
		return -1;
	}

	if (ftpConfig(&ftp) == -1) {
		printf("ERROR: Failed to configure connection.\n");
		printUsage(argv[0]);
		return -1;
	}

	if (ftpSize(&ftp, url.file) == -1) {
		printf("ERROR: Failed to get file size.\n");
		printUsage(argv[0]);
		return -1;
	}

	if (ftpRetr(&ftp, url.file) == -1) {
		printf("ERROR: Failed to start transmission of file.\n");
		printUsage(argv[0]);
		return -1;
	}

	printf("\n");
	if (ftpDownload(&ftp, url.file) == -1) {
		printf("ERROR: Failed to receive file.\n");
		printUsage(argv[0]);
		return -1;
	}
	printf("\n\n");

	if (ftpClose(&ftp) == -1) {
		printf("ERROR: Failed to close connection.\n");
		printUsage(argv[0]);
		return -1;
	}

	return 0;
}