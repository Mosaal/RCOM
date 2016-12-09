#ifndef FTP_H
#define FTP_H

#include "Utils.h"

typedef struct {
	int dataSocketFd;
	int controlSocketFd;
} FTP;

int initFTP(FTP *ftp, const char *ip);
int connectSocket(const char *ip, int port);

int ftpLogin(FTP *ftp, const char *user, const char *pass);
int ftpCWD(FTP *ftp, const char *path);
int ftpPasv(FTP *ftp);
// int ftpConfig(); mode s; type i
// int ftpSize(); size <file>
int ftpRetr();

int ftpRead(FTP *ftp, char *str, size_t size);
int ftpWrite(FTP *ftp, char *str, size_t size);

#endif