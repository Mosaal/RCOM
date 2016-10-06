#ifndef DATALINK_H_
#define DATALINK_H_

#include <stdlib.h>
#include <termios.h>
#include "Utils.h"
#include "Application.h"

typedef struct {
	char *port;
	char frame[MAX_SIZE];
	int mode, baudrate, max_size;
	struct termios oldtio, newtio;
	unsigned int ns, timeout, retries;
} DataLink;

DataLink *dl;

int initDataLink(int mode, char *port) {
	dl = (DataLink *)malloc(sizeof(DataLink));

	dl->ns = 0;
	dl->mode = mode;
	dl->port = port;
	dl->timeout = TIMEOUT;
	dl->retries = RETRIES;
	dl->baudrate = BAUDRATE;
	dl->max_size = MAX_SIZE;

	// Save current port settings
	if (tcgetattr(app->fd, &dl->oldtio) != 0) {
		printf("ERROR: Could not save current port settings.\n");
		return -1;
	}

	// Set new termios structure
	bzero(&dl->newtio, sizeof(dl->newtio));
	dl->newtio.c_cflag = dl->baudrate | CS8 | CLOCAL | CREAD;
	dl->newtio.c_iflag = IGNPAR;
	dl->newtio.c_oflag = 0;
	dl->newtio.c_lflag = 0;
	dl->newtio.c_cc[VMIN] = 0;
	dl->newtio.c_cc[VTIME] = 3;

	tcflush(app->fd, TCIOFLUSH);
	if (tcsetattr(app->fd, TCSANOW, &dl->newtio) == -1) {
		printf("ERROR: Failed to set new termios structure.\n");
		return -1;
	}

	return 0;
}

int closeSerialPort() {
	if (tcsetattr(app->fd, TCSANOW, &dl->oldtio) == -1) {
		perror("tcsetattr");
		return -1;
	}

	close(app->fd);

	return 0;
}

int llopen() {
	switch (app->mode) {
	case SEND:
		int tries = 0, connected = 0;
		break;
	case RECEIVE:
		break;
	}

	return 0;
}

int llread();

int llwrite();

int llclose();

#endif /* DATALINK_H_ */
