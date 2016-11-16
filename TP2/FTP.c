#include <stdlib.h>
#include "FTP.h"

FTP *initFTP() {
	FTP *ftp = (FTP *)malloc(sizeof(FTP));
	return ftp;
}