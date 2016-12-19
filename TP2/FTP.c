#include "FTP.h"

int initFTP(FTP *ftp, const char *ip) {
	int socketFd;
	char rd[1024];

	if ((socketFd = connectSocket(ip, PORT)) < 0) {
		printf("ERROR: Cannot connect to socket.\n");
		return -1;
	}

	ftp->dataSocketFd = 0;
	ftp->controlSocketFd = socketFd;

	if (ftpRead(ftp, rd, sizeof(rd)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	return 0;
}

int connectSocket(const char *ip, int port) {
	int socketFd;
	struct sockaddr_in server_addr;

	// server address handling
	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */

	// open an TCP socket
	if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return -1;
	}

	// connect to the server
	if (connect(socketFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("connect()");
		return -1;
	}

	return socketFd;
}

int ftpLogin(FTP *ftp, const char *user, const char *pass) {
	char sd[1024];

	// username
	sprintf(sd, "user %s\r\n", user);

	if (ftpWrite(ftp, sd, strlen(sd)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, sd, sizeof(sd)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	// clean buffer
	memset(sd, 0, sizeof(sd));

	// password
	sprintf(sd, "pass %s\r\n", pass);

	if (ftpWrite(ftp, sd, strlen(sd)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, sd, sizeof(sd)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	return 0;
}

int ftpCWD(FTP *ftp, const char *path) {
	char cwd[1024];
	sprintf(cwd, "cwd %s\r\n", path);

	if (ftpWrite(ftp, cwd, strlen(cwd)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, cwd, sizeof(cwd)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	return 0;
}

int ftpPasv(FTP *ftp) {
	char pasv[1024] = "pasv\r\n";

	if (ftpWrite(ftp, pasv, strlen(pasv)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, pasv, sizeof(pasv)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	int i = 0, result[6];

	strcpy(pasv, pasv + 27);
	pasv[strlen(pasv) - 3] = '\0';
	char *t = strtok(pasv, ",");

	while (t != NULL) {
		result[i++] = atoi(t);
		t = strtok(NULL, ",");
	}

	memset(pasv, 0, sizeof(pasv));
	sprintf(pasv, "%d.%d.%d.%d", result[0], result[1], result[2], result[3]);

	int port = (result[4] * 256) + result[5];

	if ((ftp->dataSocketFd = connectSocket(pasv, port)) == -1) {
		printf("ERROR: Invalid port.\n");
		return -1;
	}

	return 0;
}

int ftpConfig(FTP *ftp) {
	char conf[1024] = "mode s\r\n";

	// mode s
	if (ftpWrite(ftp, conf, strlen(conf)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, conf, sizeof(conf)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	// clean buffer
	memset(conf, 0, sizeof(conf));

	// type i
	strcpy(conf, "type i\r\n");

	if (ftpWrite(ftp, conf, strlen(conf)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, conf, sizeof(conf)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	return 0;
}

int ftpSize(FTP *ftp, const char *fileName) {
	char size[1024];
	sprintf(size, "size %s\r\n", fileName);

	if (ftpWrite(ftp, size, strlen(size)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, size, sizeof(size)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	strcpy(size, size + 4);
	ftp->fileSize = atoi(size);

	return 0;
}

int ftpRetr(FTP *ftp, const char *fileName) {
	char retr[1024];
	sprintf(retr, "retr %s\r\n", fileName);

	if (ftpWrite(ftp, retr, strlen(retr)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, retr, sizeof(retr)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	return 0;
}

int ftpDownload(FTP *ftp, const char *fileName) {
	FILE *file = fopen(fileName, "wb");

	if (file == NULL) {
		printf("ERROR: Failed to create file '%s'\n", fileName);
		return -1;
	}

	char buf[1024];
	int readBytes = 0, totalBytes = 0;

	while ((readBytes = read(ftp->dataSocketFd, buf, sizeof(buf)))) {
		totalBytes += readBytes;
		fwrite(buf, readBytes, 1, file);
		printProgress(totalBytes, ftp->fileSize);
	}

	fclose(file);
	return 0;
}

int ftpClose(FTP *ftp) {
	char disc[1024];

	if (ftpRead(ftp, disc, sizeof(disc)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	sprintf(disc, "quit\r\n");

	if (ftpWrite(ftp, disc, strlen(disc)) == -1) {
		printf("ERROR: Write failure.\n");
		return -1;
	}

	if (ftpRead(ftp, disc, sizeof(disc)) == -1) {
		printf("ERROR: Read failure.\n");
		return -1;
	}

	if (ftp->controlSocketFd)
		close(ftp->controlSocketFd);
	
	return 0;
}

int ftpRead(FTP *ftp, char *str, size_t size) {
	FILE *file = fdopen(ftp->controlSocketFd, "r");

	if (file == NULL) {
		printf("ERROR: Failed to open socket.\n");
		return -1;
	}

	do {
		memset(str, 0, size);
		str = fgets(str, size, file);
		printf("%s", str);
	} while (!('1' <= str[0] && str[0] <= '5') || str[3] != ' ');

	return 0;
}

int ftpWrite(FTP *ftp, char *str, size_t size) {
	if (write(ftp->controlSocketFd, str, size) <= 0) {
		printf("ERROR: Failed to write %s to the socket.\n", str);
		return -1;
	}

	return 0;
}

void printProgress(float curr, float total) {
	float per = (100.0 * curr) / total;
	printf("Completed: %6.2f%% [", per);

	int i, pos = (per * PROGRESS_BAR) / 100.0;
	for (i = 0; i < PROGRESS_BAR; i++) {
		if (i <= pos)
			printf("=");
		else
			printf(" ");
	}

	printf("]\r");
	fflush(stdout);
}