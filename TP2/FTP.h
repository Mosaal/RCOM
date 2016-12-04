#ifndef FTP_H
#define FTP_H

#include "Utils.h"

typedef struct {
	int dataSocketFd;
	int controlSocketFd;
} FTP;

void initFTP(FTP *ftp);

#endif