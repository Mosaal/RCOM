#ifndef FTP_H
#define FTP_H

typedef struct {
	int dataSocketFd;
	int controlSocketFd;
} FTP;

FTP *initFTP();

#endif