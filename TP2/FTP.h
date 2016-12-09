#ifndef FTP_H
#define FTP_H

#define PORT         21
#define PROGRESS_BAR 50

#include <stdio.h>
#include <regex.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
	int fileSize;
	int dataSocketFd;
	int controlSocketFd;
} FTP;

int initFTP(FTP *ftp, const char *ip);
int connectSocket(const char *ip, int port);

int ftpLogin(FTP *ftp, const char *user, const char *pass);
int ftpCWD(FTP *ftp, const char *path);
int ftpPasv(FTP *ftp);
int ftpConfig(FTP *ftp);
int ftpSize(FTP *ftp, const char *fileName);
int ftpRetr(FTP *ftp, const char *fileName);
int ftpDownload(FTP *ftp, const char *fileName);
int ftpClose(FTP *ftp);

int ftpRead(FTP *ftp, char *str, size_t size);
int ftpWrite(FTP *ftp, char *str, size_t size);

void printProgress(float curr, float total);

#endif