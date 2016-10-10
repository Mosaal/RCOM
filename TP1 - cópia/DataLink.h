#ifndef DATALINK_H
#define DATALINK_H

#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"

volatile int STOP = FALSE;

int initDataLink(Application *app, DataLink *dl) {
	dl = (DataLink *)malloc(sizeof(DataLink));

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

	/*switch (app->mode) {
	case SEND:
		dl->newtio.c_cc[VMIN] = 0;
		dl->newtio.c_cc[VTIME] = 30;
		break;
	case RECEIVE:
		dl->newtio.c_cc[VMIN] = 5;
		dl->newtio.c_cc[VTIME] = 10;
		break;
	}*/

	tcflush(app->fd, TCIOFLUSH);
	if (tcsetattr(app->fd, TCSANOW, &dl->newtio) == -1) {
		printf("ERROR: Failed to set new termios structure.\n");
		return -1;
	}

	return 0;
}

int llopen(int fd, int mode) {
	int res = 0;
	unsigned char UA[5];
	unsigned char SET[5];
	unsigned char x, flag, bcc;

	switch (mode) {
	case SEND:
		SET[0] = FLAG;
		SET[1] = A_TR;
		SET[2] = C_SET;
		SET[3] = SET[1] ^ SET[2];
		SET[4] = FLAG;

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

		UA[0] = FLAG;
		UA[1] = A_RT;
		UA[2] = C_UA;
		UA[3] = UA[1] ^ UA[2];
		UA[4] = FLAG;

		if (write(fd, UA, sizeof(UA)) == -1) {
			printf("ERROR: Failed to send UA buffer.\n");
			return -1;
		}
		break;
	}

	return 0;
}

int llwrite(int fd, unsigned char *buffer, int length) {
	int i, XOR = 0;
	unsigned char DATA[length + 5];

	for (i = 0; i < length; i++)
		XOR ^= buffer[i];

	DATA[0] = FLAG;
	DATA[1] = A_TR;

	DATA[3] = DATA[1] ^ DATA[2];
}

int llread(int fd, unsigned char *buffer) {

}

int llclose(int fd, int mode) {
	int res = 0;
	unsigned char x, flag, bcc;
	unsigned char UA[5], DISC_TR[5], DISC_RT[5];

	switch (mode) {
	case SEND:
		DISC_TR[0] = FLAG;
		DISC_TR[1] = A_TR;
		DISC_TR[2] = C_DISC;
		DISC_TR[3] = DISC_TR[1] ^ DISC_TR[2];
		DISC_TR[4] = FLAG;

		if (write(fd, DISC_TR, sizeof(DISC_TR)) == -1) {
			printf("ERROR: Failed to send DISC buffer.\n");
			return -1;
		}

		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			DISC_RT[res - 1] = x;
		}

		bcc = DISC_RT[1] ^ DISC_RT[2];
		if (DISC_RT[3] != bcc) {
			printf("ERROR: Failure on closing connection.\n");
			return -1;
		}

		UA[0] = FLAG;
		UA[1] = A_RT;
		UA[2] = C_UA;
		UA[3] = UA[1] ^ UA[2];
		UA[4] = FLAG;

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

			DISC_RT[res - 1] = x;
		}

		bcc = DISC_RT[1] ^ DISC_RT[2];
		if (DISC_RT[3] != bcc) {
			printf("ERROR: Failure on closing connection.\n");
			return -1;
		}

		DISC_TR[0] = FLAG;
		DISC_TR[1] = A_RT;
		DISC_TR[2] = C_DISC;
		DISC_TR[3] = DISC_TR[1] ^ DISC_TR[2];
		DISC_TR[4] = FLAG;

		if (write(fd, DISC_TR, sizeof(DISC_TR)) == -1) {
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

int closeSerialPort(Application *app, DataLink *dl) {
	if (tcsetattr(app->fd, TCSANOW, &dl->oldtio) == -1) {
		perror("tcsetattr");
		return -1;
	}

	close(app->fd);
	return 0;
}

#endif