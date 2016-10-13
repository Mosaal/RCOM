#ifndef DATALINK_H
#define DATALINK_H

#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"
#include "Alarm.h"
#include "ByteStuffing.h"

volatile int STOP = FALSE;

int initDataLink() {
	dl = (DataLink *)malloc(sizeof(DataLink));

	dl->retries = 3;
	dl->timeout = 3;

	// Save current port settings
	if (tcgetattr(app->fd, &dl->oldtio) == -1) {
		printf("ERROR: Could not save current port settings.\n");
		return -1;
	}

	// Set new termios structure
	bzero(&dl->newtio, sizeof(dl->newtio));
	dl->newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	dl->newtio.c_iflag = IGNPAR;
	dl->newtio.c_oflag = 0;
	dl->newtio.c_lflag = 0;
	dl->newtio.c_cc[VMIN] = 1;
	dl->newtio.c_cc[VTIME] = 0;

	tcflush(app->fd, TCIOFLUSH);
	if (tcsetattr(app->fd, TCSANOW, &dl->newtio) == -1) {
		printf("ERROR: Failed to set new termios structure.\n");
		return -1;
	}

	return 0;
}

int llopen(int fd, int mode) {
	int res = 0;
	unsigned char x, flag, bcc;

	switch (mode) {
	case SEND:
		(void)signal(SIGALRM, reconnect);
		alarm(dl->timeout);

		if (write(fd, SET, sizeof(SET)) == -1) {
			printf("ERROR: Failed to send SET buffer.\n");
			return -1;
		}

		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			UA[res - 1] = x;
		}

		bcc = UA[1] ^ UA[2];
		if (UA[3] != bcc) {
			printf("ERROR: Failure on initial connection.\n");
			return -1;
		}

		alarm(0);
		break;
	case RECEIVE:
		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			SET[res - 1] = x;
		}

		bcc = SET[1] ^ SET[2];
		if (SET[3] != bcc) {
			printf("ERROR: Failure on initial connection.\n");
			return -1;
		}

		if (write(fd, UA, sizeof(UA)) == -1) {
			printf("ERROR: Failed to send UA buffer.\n");
			return -1;
		}
		break;
	}

	return 0;
}

int llwrite(int fd, unsigned char *buffer, int length) {

}

int llread(int fd, unsigned char *buffer) {

}

int llclose(int fd, int mode) {
	int res = 0;
	unsigned char x, flag, bcc;

	switch (mode) {
	case SEND:
		if (write(fd, DISC, sizeof(DISC)) == -1) {
			printf("ERROR: Failed to send DISC buffer.\n");
			return -1;
		}

		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			DISC[res - 1] = x;
		}

		bcc = DISC[1] ^ DISC[2];
		if (DISC[3] != bcc) {
			printf("ERROR: Failure on closing connection.\n");
			return -1;
		}

		if (write(fd, UA, sizeof(UA)) == -1) {
			printf("ERROR: Failed to send UA buffer.\n");
			return -1;
		}
		break;
	case RECEIVE:
		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			DISC[res - 1] = x;
		}

		bcc = DISC[1] ^ DISC[2];
		if (DISC[3] != bcc) {
			printf("ERROR: Failure on closing connection.\n");
			return -1;
		}

		if (write(fd, DISC, sizeof(DISC)) == -1) {
			printf("ERROR: Failed to send DISC buffer.\n");
			return -1;
		}

		res = 0;
		STOP = FALSE;
		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			UA[res - 1] = x;
		}

		bcc = UA[1] ^ UA[2];
		if (UA[3] != bcc) {
			printf("ERROR: Failure on initial connection.\n");
			return -1;
		}
		break;
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

#endif