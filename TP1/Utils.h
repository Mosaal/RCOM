#ifndef UTILS_H
#define UTILS_H

#define TRUE           1
#define FALSE          0

#define SEND           0
#define RECEIVE        1

#define A              0x03
#define A_TR           0x03
#define A_RT           0x01
#define FLAG           0x7E
#define ESCAPE         0x7D
#define FLAG_NEXT      0x5E
#define ESCAPE_NEXT    0x5D

#define C_S0           0x00
#define C_S1           0x20
#define C_UA           0x03
#define C_SET          0x07
#define C_DISC         0x0B

#define C_RR0          0x01
#define C_RR1          0x21
#define C_REJ0         0x05
#define C_REJ1         0x25

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
	int retries, timeout;
	struct termios oldtio, newtio;
} DataLink;

DataLink *dl;
Application *app;

unsigned char UA[] = { FLAG, A, C_UA, A ^ C_UA, FLAG };
unsigned char SET[] = { FLAG, A, C_SET, A ^ C_SET, FLAG };
unsigned char DISC[] = { FLAG, A, C_DISC, A ^ C_DISC, FLAG };

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