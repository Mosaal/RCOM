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

typedef struct {
	char *file, *port;
	int fd, mode;
} Application;

//int send_frame(char* sframe, char*)

int initDataLink(Application *app, DataLink *dl) {
	dl->ns = 0;
	dl->mode = app->mode;
	dl->port = app->port;
	dl->timeout = TIMEOUT;
	dl->retries = RETRIES;
	dl->baudrate = BAUDRATE;
	dl->max_size = MAX_SIZE;

	// Save current port settings
	if (tcgetattr(app->fd, &dl->oldtio) == -1) {
		printf("ERROR: Could not save current port settings.\n");
		return -1;
	}

	// Set new termios structure
	bzero(&dl->newtio, sizeof(dl->newtio));
	dl->newtio.c_cflag = dl->baudrate | CS8 | CLOCAL | CREAD;
	dl->newtio.c_iflag = IGNPAR;
	dl->newtio.c_oflag = 0;
	dl->newtio.c_lflag = 0;
	if(app->mode == SEND)
	{
		dl->newtio.c_cc[VMIN] = 10;
		dl->newtio.c_cc[VTIME] = 0;
	}
	else if(app->mode == RECEIVE)
	{
		dl->newtio.c_cc[VMIN] = 10;
		dl->newtio.c_cc[VTIME] = 5;
	}
	else
		return(-1);
	tcflush(app->fd, TCIOFLUSH);
	if (tcsetattr(app->fd, TCSANOW, &dl->newtio) == -1) {
		printf("ERROR: Failed to set new termios structure.\n");
		return -1;
	}

	return 0;
}

int closeSerialPort(Application *app, DataLink *dl) {
	if (tcsetattr(app->fd, TCSANOW, &dl->oldtio) == -1) {
		perror("tcsetattr");
		return -1;
	}

	close(app->fd);

	return 0;
}

int llopen() {
	/*switch (app->mode) {
	case SEND:
		int tries = 0, connected = 0;
		break;
	case RECEIVE:
		break;
	}
*/
	return 0;
}

int llread();

int llwrite();

int llclose();

#endif /* DATALINK_H_ */
