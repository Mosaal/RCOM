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

	dl->ns = 0;
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

unsigned char *createCommand(int command) {
	unsigned char *buf = (unsigned char *)malloc(5);

	buf[0] = FLAG;
	buf[1] = A;

	switch (command) {
	case UA:
		buf[2] = C_UA;
		break;
	case SET:
		buf[2] = C_SET;
		break;
	case DISC:
		buf[2] = C_DISC;
		break;
	case RR:
		break;
	case REJ:
		break;
	}

	buf[3] = buf[1] ^ buf[2];
	buf[4] = FLAG;

	return buf;
}

unsigned char *createFrame(unsigned char *buffer, int length) {
	int i;
	unsigned char BCC2 = 0x00;

	FRAME_SIZE = length + 6;
	unsigned char *frame = (unsigned char *)malloc(length + 6);

	frame[0] = FLAG;
	frame[1] = A;
	frame[2] = dl->ns << 6;
	frame[3] = frame[1] ^ frame[2];

	for (i = 0; i < length; i++) {
		BCC2 ^= buffer[i];
		frame[i + 4] = buffer[i];
	}

	frame[length + 4] = BCC2;
	frame[length + 5] = FLAG;

	return frame;
}

int llopen(int fd, int mode) {
	int res = 0;
	unsigned char x, flag;
	unsigned char answer[5];

	switch (mode) {
	case SEND:
		COMMAND = createCommand(SET);

		(void)signal(SIGALRM, connect);
		alarm(dl->timeout);

		if (write(fd, COMMAND, COMMAND_SIZE) == -1) {
			printf("ERROR: Failed to send SET buffer.\n");
			return -1;
		}

		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			answer[res - 1] = x;
		}

		if (answer[3] != (A ^ C_UA)) {
			printf("ERROR: Failure on initial connection.\n");
			return -1;
		}

		alarm(0);
		break;
	case RECEIVE:
		COMMAND = createCommand(UA);

		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			answer[res - 1] = x;
		}

		if (answer[3] != (A ^ C_SET)) {
			printf("ERROR: Failure on initial connection.\n");
			return -1;
		}

		if (write(fd, COMMAND, COMMAND_SIZE) == -1) {
			printf("ERROR: Failed to send UA buffer.\n");
			return -1;
		}
		break;
	}

	return 0;
}

int llwrite(int fd, unsigned char *buffer, int length) {
	int res = 0;
	unsigned char x, flag;
	unsigned char answer[5];

	FRAME = createFrame(buffer, length);

	// stuff it
	(void)signal(SIGALRM, send);
	alarm(dl->timeout);

	if (write(fd, FRAME, FRAME_SIZE) == -1)
		return -1;

	STOP = FALSE;
	while (STOP == FALSE) {
		res += read(fd, &x, 1);

		if (res == 1)
			flag = x;
		if (x == flag && res > 1)
			STOP = TRUE;

		answer[res - 1] = x;
	}

	if (answer[3] == (A ^ C_RR)) {
		alarm(0);
		triesSend = 0;
		return FRAME_SIZE;
	} else if (answer[3] == (A ^ C_REJ)) {
		alarm(0);
		return -1;
	}
}

int llread(int fd, unsigned char **buffer) {
	
}

int llclose(int fd, int mode) {
	int res = 0;
	unsigned char x, flag;
	unsigned char answer[5];

	switch (mode) {
	case SEND:
		COMMAND = createCommand(DISC);

		if (write(fd, COMMAND, COMMAND_SIZE) == -1) {
			printf("ERROR: Failed to send DISC buffer.\n");
			return -1;
		}

		STOP = FALSE;
		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			answer[res - 1] = x;
		}

		if (answer[3] != (A ^ C_DISC)) {
			printf("ERROR: Failure on closing connection.\n");
			return -1;
		}

		COMMAND = createCommand(UA);
		if (write(fd, COMMAND, COMMAND_SIZE) == -1) {
			printf("ERROR: Failed to send UA buffer.\n");
			return -1;
		}
		break;
	case RECEIVE:
		STOP = FALSE;
		while (STOP == FALSE) {
			res += read(fd, &x, 1);

			if (res == 1)
				flag = x;
			if (x == flag && res > 1)
				STOP = TRUE;

			answer[res - 1] = x;
		}

		if (answer[3] != (A ^ C_DISC)) {
			printf("ERROR: Failure on closing connection.\n");
			return -1;
		}

		COMMAND = createCommand(DISC);
		if (write(fd, COMMAND, COMMAND_SIZE) == -1) {
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

			answer[res - 1] = x;
		}

		if (answer[3] != (A ^ C_UA)) {
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