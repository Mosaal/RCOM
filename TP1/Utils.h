#ifndef UTILS_H
#define UTILS_H

#define TRUE             1
#define FALSE            0

#define SEND             0
#define RECEIVE          1

#define UA               0
#define SET              1
#define DISC             2

#define A                0x03
#define FLAG             0x7E
#define DATA             0x02
#define ESCAPE           0x7D

#define C_UA             0x03
#define C_SET            0x07
#define C_DISC           0x0B

#define COMMAND_SIZE     5
#define DATA_PKG_SIZE    5
#define CTRL_PKG_SIZE    13

#define CTRL_PKG_START   1
#define CTRL_PKG_DATA    2
#define CTRL_PKG_END     3

#define PARAM_FILE_SIZE  0
#define PARAM_FILE_NAME  1

#define _POSIX_SOURCE    1
#define PROGRESS_BAR     30
#define MAX_SIZE         512
#define BAUDRATE         B38400

#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>

typedef enum {
	START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, DONE
} State;

typedef struct {
	int fd, mode;
	char *port, *fileName;
} Application;

typedef struct {
	int ns, retries, timeout;
	struct termios oldtio, newtio;
} DataLink;

DataLink *dl;
Application *app;

long int getFileSize(char *fileName) {
	struct stat st;

	if (stat(fileName, &st) == 0)
		return st.st_size;

	printf("ERROR: Could not get file size.\n");
	return -1;
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

	printf("]\n");
}

#endif