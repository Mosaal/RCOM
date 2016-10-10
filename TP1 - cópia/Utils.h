#ifndef UTILS_H
#define UTILS_H

#define TRUE           1
#define FALSE          0

#define SEND           0
#define RECEIVE        1

#define A_TR           0x03
#define A_RT           0x01
#define FLAG           0x7E
#define ESCAPE         0x7D

#define C_UA           0x03
#define C_SET          0x07
#define C_DISC         0x0B

#define MAX_SIZE       512
#define BAUDRATE       B38400
#define _POSIX_SOURCE  1

#include <stdio.h>
#include <termios.h>

typedef struct {
	int fd, mode;
	char *port, *fileName;
} Application;

typedef struct {
	struct termios oldtio, newtio;
} DataLink;

long int getFileSize(FILE *file) {
	if (fseek(file, 0, SEEK_END) == -1) {
		printf("ERROR: Could not get file size.\n");
		return -1;
	}

	long int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	return size;
}

#endif