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

int sendControlPackage(int ctrl, unsigned char *buffer, int length) {
	int i;
	unsigned char ctrlPackage[CTRL_PKG_SIZE];

	ctrlPackage[0] = ctrl;
	ctrlPackage[1] = PARAM_FILE_SIZE;
	ctrlPackage[2] = length;

	for (i = 0; i < length; i++)
		ctrlPackage[i + 3] = buffer[i];

	if (llwrite(app->fd, ctrlPackage, CTRL_PKG_SIZE) == -1)
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

	if (llwrite(app->fd, dataPackage, length + DATA_PKG_SIZE) == -1)
		return -1;

	return 0;
}

int receiveControlPackage(int ctrl, long int *size) {
	unsigned char x, flag;
	int res = 0, var = FALSE;
	unsigned char *answer;

	if (llread(app->fd, &answer) == -1)
		return -1;

	if (answer[4] == CTRL_PKG_START)
		printf("Received START control package.\n");
	else if (answer[4] == CTRL_PKG_END)
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

int receiveDataPackage(unsigned char *buf) {
	int i, readBytes = 0;
	unsigned char *package;

	readBytes = llread(app->fd, &package);
	if (readBytes == -1)
		return -1;

	for (i = 0; i < readBytes; i++)
		buf[i] = package[i + 8];

	return readBytes;
}

int sendFile(FILE *file) {
	long int fileSize = getFileSize(app->fileName);
	unsigned char *fileSizeBuf = (unsigned char *)&fileSize;

	// printf("size = %ld\n", fileSize);
	// printBuffer(fileSizeBuf, 8);
	// printf("\n");

	if (sendControlPackage(CTRL_PKG_START, fileSizeBuf, sizeof(fileSizeBuf)) == -1) {
		printf("ERROR: Failed to send the START control package.\n");
		return -1;
	}

	unsigned char fileBuf[MAX_SIZE];
	size_t readBytes = 0, writtenBytes = 0, i = 0;
	while ((readBytes = fread(fileBuf, sizeof(unsigned char), MAX_SIZE, file)) > 0) {
		if (sendDataPackage(i++, fileBuf, readBytes) == -1) {
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

int receiveFile(FILE *file) {
	long int fileSize;
	if (receiveControlPackage(CTRL_PKG_START, &fileSize) == -1) {
		printf("ERROR: Failed to receive the START control package.\n");
		return -1;
	}

	// printf("fileSize = %lu\n", fileSize);
	unsigned char tempBuf[MAX_SIZE];//, fileBuf[fileSize];

	size_t readBytes = 0, receivedBytes = 0, i = 0;
	while (readBytes != fileSize) {
		receivedBytes = receiveDataPackage(tempBuf);

		fwrite(tempBuf, 1, receivedBytes, file);

		readBytes += receivedBytes;
		printProgress(readBytes, fileSize);
	}

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
		FILE *file;
		file = fopen(app->fileName, "wb");
		if (file == NULL) {
			printf("ERROR: Failed to create file \"%s\".\n", app->fileName);
			return -1;
		}

		if (receiveFile(file) == -1) {
			printf("ERROR: Failed to receive file \"%s\".\n", app->fileName);
			return -1;
		}

		if (fclose(file) != 0) {
			printf("ERROR: Failed to close file \"%s\".\n", app->fileName);
			return -1;
		}
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