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
		printf("0x%02x\n", buf[i]);
}

int sendControlPackage(int ctrl, unsigned char *buffer, int length) {
	int i;
	unsigned char ctrlPackage[CTRL_PKG_SIZE];

	ctrlPackage[0] = FLAG;
	ctrlPackage[1] = A;
	ctrlPackage[2] = ctrl;

	for (i = 0; i < length; i++)
		ctrlPackage[i + 3] = buffer[i];

	ctrlPackage[length + 3] = ctrlPackage[1] ^ ctrlPackage[2];
	ctrlPackage[length + 4] = FLAG;

	if (write(app->fd, ctrlPackage, CTRL_PKG_SIZE) == -1)
		return -1;
	
	if (ctrl == CTRL_PKG_START)
		printf("Sent START control package.\n");
	else if (ctrl == CTRL_PKG_END)
		printf("Sent END control package.\n");

	return 0;
}

int sendDataPackage(unsigned char *buffer, int length) {
	int i;
	unsigned char dataPackage[length + DATA_PKG_SIZE];
	unsigned char *readBytesBuf = (unsigned char *)&length;

	dataPackage[0] = FLAG;
	dataPackage[1] = A;
	dataPackage[2] = CTRL_PKG_DATA;

	for (i = 0; i < length; i++)
		dataPackage[i + 3] = buffer[i];

	dataPackage[length + 3] = dataPackage[1] ^ dataPackage[2];
	dataPackage[length + 4] = FLAG;

	if (write(app->fd, dataPackage, length + DATA_PKG_SIZE) == -1)
		return -1;

	return 0;
}

int receiveControlPackage(int ctrl, long int *size) {
	unsigned char x, flag;
	int res = 0, var = FALSE;
	unsigned char answer[CTRL_PKG_SIZE];

	while (var == FALSE) {
		res += read(app->fd, &x, 1);

		if (res == 1)
			flag = x;
		if (x == flag && res == CTRL_PKG_SIZE)
			var = TRUE;

		answer[res - 1] = x;
	}

	if (answer[11] == (A ^ CTRL_PKG_START))
		printf("Received START control package.\n");
	else if (answer[11] == (A ^ CTRL_PKG_END))
		printf("Received END control package\n");
	else
		return -1;

	int i;
	unsigned char fileSizeBuf[8];
	for (i = 0; i < 8; i++)
		fileSizeBuf[i] = answer[i + 3];
	memcpy(size, fileSizeBuf, 8);

	return 0;
}

int receiveDataPackage() {
	long int size = 0;
	State state = START;
	volatile int done = FALSE;
	unsigned char fileBuf[MAX_SIZE];

	while (done == FALSE) {
		unsigned char x;

		if (state != DONE)
			read(app->fd, &x, 1);

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
			} else {
				size = 0;
				state = START;
			}
			break;
		case A_RCV:
			if (x == DATA) {
				fileBuf[size++] = x;
				state = C_RCV;
			} else if (x == FLAG) {
				size = 1;
				state = FLAG_RCV;
			} else {
				size = 0;
				state = START;
			}
			break;
		case C_RCV:
			if (x == (A ^ DATA)) {
				fileBuf[size++] = x;
				state = BCC_OK;
			} else {
				fileBuf[size++] = x;
			}
			break;
		case BCC_OK:
			if (x == FLAG) {
				fileBuf[size++] = x;
				state = DONE;
			} else {
				fileBuf[size++] = x;
				// state = C_RCV;
			}
			break;
		case DONE:
			done = TRUE;
			break;
		}

		printf("0x%02x\n", x);
		printf("size = %ld\n", size);
	}

	return size - 5;
}

int sendFile(FILE *file) {
	long int fileSize = getFileSize(app->fileName);
	unsigned char *fileSizeBuf = (unsigned char *)&fileSize;

	if (sendControlPackage(CTRL_PKG_START, fileSizeBuf, sizeof(fileSizeBuf)) == -1) {
		printf("ERROR: Failed to send the START control package.\n");
		return -1;
	}

	unsigned char fileBuf[MAX_SIZE];
	size_t readBytes = 0, writtenBytes = 0, i = 0;
	while ((readBytes = fread(fileBuf, sizeof(unsigned char), MAX_SIZE, file)) > 0) {
		if (sendDataPackage(fileBuf, readBytes) == -1) {
			printf("ERROR: Failed to send one of the DATA packages.\n");
			return -1;
		}

		memset(fileBuf, 0, MAX_SIZE);
		writtenBytes += readBytes;

		printProgress(writtenBytes, fileSize);
	}

	if (sendControlPackage(CTRL_PKG_END, fileSizeBuf, sizeof(fileSizeBuf)) == -1) {
		printf("ERROR: Failed to send the START control package.\n");
		return -1;
	}

	return 0;
}

int receiveFile() {
	long int fileSize;
	if (receiveControlPackage(CTRL_PKG_START, &fileSize) == -1) {
		printf("ERROR: Failed to receive the START control package.\n");
		return -1;
	}

	int readBytes = 0, receivedBytes = 0;
	while ((readBytes = receiveDataPackage()) != fileSize) {
		receivedBytes += readBytes;
		printProgress(receivedBytes, fileSize);
	}

	if (receivedBytes == 0)
		printProgress(readBytes, fileSize);

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