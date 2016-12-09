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

	int port, port1, port2;
	int ip1, ip2, ip3, ip4;

	sscanf("227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &port1, &port2);
	memset(pasv, 0, sizeof(pasv));
	sprintf(pasv, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

	port = (port1 * 256) + port2;
	printf("IP: %s; PORT: %d\n", pasv, port);
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