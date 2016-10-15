#ifndef APPLICATION_H
#define APPLICATION_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Utils.h"
#include "DataLink.h"
#include "ByteStuffing.h"

void printBuffer(unsigned char *buf, int size) {
	int i;
	for (i = 0; i < size; i++)
		printf("0x%02x ", buf[i]);
}

unsigned char *createFrame(int C, unsigned char *buffer, int length) {
	int i;
	unsigned char BCC2 = 0x00;
	unsigned char *frame = (unsigned char *)malloc(length + 6);

	frame[0] = FLAG;
	frame[1] = A;
	frame[2] = C;
	frame[3] = frame[1] ^ frame[2];

	for (i = 0; i < length; i++) {
		BCC2 ^= buffer[i];
		frame[i + 4] = buffer[i];
	}

	frame[length + 4] = BCC2;
	frame[length + 5] = FLAG;

	return frame;
}

int sendControlPackage(int ctrl, unsigned char *buffer, int length) {
	int i, FRAME_SIZE = CTRL_PKG_SIZE + 6;
	unsigned char ctrlPackage[CTRL_PKG_SIZE];

	ctrlPackage[0] = ctrl;
	ctrlPackage[1] = PARAM_FILE_SIZE;
	ctrlPackage[2] = length;

	for (i = 0; i < length; i++)
		ctrlPackage[i + 3] = buffer[i];

	unsigned char *frame = createFrame(ctrl, ctrlPackage, CTRL_PKG_SIZE);

	if (write(app->fd, frame, FRAME_SIZE) == -1)
		return -1;
	
	if (ctrl == CTRL_PKG_START)
		printf("Sent START control package.\n");
	else if (ctrl == CTRL_PKG_END)
		printf("Sent END control package.\n");

	return 0;
}

int sendDataPackage(int N, unsigned char *buffer, int length) {
	int i, FRAME_SIZE = length + DATA_PKG_SIZE + 6;
	unsigned char dataPackage[length + DATA_PKG_SIZE];

	dataPackage[0] = CTRL_PKG_DATA;
	dataPackage[1] = N % 255;
	dataPackage[2] = length / 256;
	dataPackage[3] = length % 256;

	for (i = 0; i < length; i++)
		dataPackage[i + 4] = buffer[i];

	unsigned char *frame = createFrame(N << 6, dataPackage, length + DATA_PKG_SIZE);

	if (write(app->fd, frame, FRAME_SIZE) == -1)
		return -1;

	return 0;
}

int receiveControlPackage(int ctrl, long int *size) {
	unsigned char x, flag;
	int res = 0, var = FALSE;
	unsigned char answer[CTRL_PKG_SIZE + 6];

	while (var == FALSE) {
		res += read(app->fd, &x, 1);

		if (res == 1)
			flag = x;
		if (x == flag && res == (CTRL_PKG_SIZE + 6))
			var = TRUE;

		answer[res - 1] = x;
	}

	if (answer[3] == (A ^ CTRL_PKG_START) && answer[4] == CTRL_PKG_START)
		printf("Received START control package.\n");
	else if (answer[3] == (A ^ CTRL_PKG_END) && answer[4] == CTRL_PKG_END)
		printf("Received END control package\n");
	else
		return -1;

	int i;
	unsigned char fileSizeBuf[8];
	for (i = 0; i < 8; i++)
		fileSizeBuf[i] = answer[i + 7];
	memcpy(size, fileSizeBuf, 8);

	return 0;
}

int receiveDataPackage() {
	int size = 0;
	int numBytes = 0, res = 0;
	unsigned char fileBuf[MAX_SIZE + 10];
	volatile int over = FALSE, state = START;

	while (over == FALSE) {
		unsigned char x;

		if (state != DONE)
			res = read(app->fd, &x, 1);
		if (size == 8)
			numBytes = fileBuf[6] * 256 + fileBuf[7];

		switch (state) {
		case START:
			if (x == FLAG) {
				fileBuf[size++] = x;
				state = FLAG_RCV;
			}
			break;
		case FLAG_RCV:
			if (x == A) {
				fileBuf[size++] = x;
				state = A_RCV;
			}
			break;
		case A_RCV:
			if (x != FLAG) {
				fileBuf[size++] = x;
				state = C_RCV;
			}
			break;
		case C_RCV:
			if (x == (fileBuf[1] ^ fileBuf[2])) {
				fileBuf[size++] = x;
				state = BCC_OK;
			}
			break;
		case BCC_OK:
			if (x == FLAG && size == (numBytes + 9)) {
				fileBuf[size++] = x;
				state = DONE;
			} else {
				fileBuf[size++] = x;
			}
			break;
		case DONE:
			over = TRUE;
			break;
		}
	}

	return size - 10;
}

int sendFile(FILE *file) {
	long int fileSize = getFileSize(app->fileName);
	unsigned char *fileSizeBuf = (unsigned char *)&fileSize;

	if (sendControlPackage(CTRL_PKG_START, fileSizeBuf, sizeof(fileSizeBuf)) == -1) {
		printf("ERROR: Failed to send the START control package.\n");
		return -1;
	}
	
	// wait for response

	unsigned char fileBuf[MAX_SIZE];
	size_t readBytes = 0, writtenBytes = 0, i = 0;
	while ((readBytes = fread(fileBuf, sizeof(unsigned char), MAX_SIZE, file)) > 0) {
		if (sendDataPackage(i++, fileBuf, readBytes) == -1) {
			printf("ERROR: Failed to send one of the DATA packages.\n");
			return -1;
		}

		// wait for response

		memset(fileBuf, 0, MAX_SIZE);
		writtenBytes += readBytes;

		printProgress(writtenBytes, fileSize);
	}

	if (sendControlPackage(CTRL_PKG_END, fileSizeBuf, sizeof(fileSizeBuf)) == -1) {
		printf("ERROR: Failed to send the START control package.\n");
		return -1;
	}
	
	// wait for response

	return 0;
}

int receiveFile() {
	long int fileSize;
	if (receiveControlPackage(CTRL_PKG_START, &fileSize) == -1) {
		printf("ERROR: Failed to receive the START control package.\n");
		return -1;
	}

	printf("size = %ld\n", fileSize);

	int readBytes = 0, receivedBytes = 0;
	while ((receivedBytes = receiveDataPackage()) != fileSize) {
		printf("receivedBytes = %d\n", receivedBytes);
		readBytes += receivedBytes;
		printProgress(readBytes, fileSize);
	}

	if (receivedBytes == fileSize)
		printProgress(receivedBytes, fileSize);

	if (receiveControlPackage(CTRL_PKG_END, &fileSize) == -1) {
		printf("ERROR: Failed to receive the END control package.\n");
		return -1;
	}

	return 0;
}

int initApplication(int mode, char *port, char *fileName) {
	app = (Application *)malloc(sizeof(Application));

	app->mode = mode;
	app->port = port;
	app->fileName = fileName;
	app->fd = open(port, O_RDWR | O_NOCTTY);

	if (app->fd < 0) {
		printf("ERROR: Failed to open serial port.\n");
		return -1;
	}

	initDataLink();

	if (llopen(app->fd, app->mode) == -1) {
		printf("ERROR: Failed to create a connection.\n");
		return -1;
	} else {
		printf("Connection successful.\n");
	}

	if (app->mode == SEND) {
		FILE *file;
		file = fopen(app->fileName, "rb");
		if (file == NULL) {
			printf("ERROR: Failed to open file \"%s\".\n", app->fileName);
			return -1;
		}

		if (sendFile(file) == -1) {
			printf("ERROR: Failed to send file \"%s\".\n", app->fileName);
			return -1;
		}

		if (fclose(file) != 0) {
			printf("ERROR: Failed to close file \"%s\".\n", app->fileName);
			return -1;
		}
	} else if (app->mode == RECEIVE) {
		/*FILE *file;
		file = fopen(app->fileName, "wb");
		if (file == NULL) {
			printf("ERROR: Failed to create file \"%s\".\n", app->fileName);
			return -1;
		}*/

		if (receiveFile() == -1) {
			printf("ERROR: Failed to receive file \"%s\".\n", app->fileName);
			return -1;
		}

		/*if (fclose(file) != 0) {
			printf("ERROR: Failed to close file \"%s\".\n", app->fileName);
			return -1;
		}*/
	}

	if (llclose(app->fd, app->mode) == -1) {
		printf("ERROR: Failed to close the connection.\n");
		return -1;
	} else {
		printf("Disconnection successful.\n");
	}

	closeSerialPort();
}

#endif