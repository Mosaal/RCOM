#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "Utils.h"
#include "DataLink.h"

typedef struct {
	char *file;
	int fd, mode;
} Application;

Application *app;

void printConfig() {
	printf("CURRENT CONFIGURATIONS\n");
//	printf(" - Mode: %s\n", app->mode);
//	printf(" - Baud rate: %d\n", ll->baudRate);
//	printf(" - Message data max. size: %d\n", ll->messageDataMaxSize);
//	printf(" - Max. no. retries: %d\n", ll->numTries - 1);
//	printf(" - Time-out interval: %d\n", ll->timeout);
//	printf(" - Port: %s\n", ll->port);
//	printf(" - File: %s\n", al->fileName);
}

int sendFile() {
	FILE *file = fopen(app->file, "rb");
	if (file == NULL) {
		printf("ERROR: Failed to open \"%s\".", app->file);
		return -1;
	}

	int fd = llopen(app->mode);

	return 0;
}

int receiveFile() {
	return 0;
}

int initApplication(int mode, char *port, char *file) {
	app = (Application *)malloc(sizeof(Application));

	app->mode = mode;
	app->file = file;
	app->fd = open(port, O_RDWR | O_NOCTTY);

	if (app->fd < 0) {
		printf("ERROR: Failed to open serial port.\n");
		return -1;
	}

	initDataLink(app->mode, app->file);

	printConfig();

	switch (app->mode) {
	case SEND:
		sendFile();
		break;
	case RECEIVE:
		receiveFile();
		break;
	}

	closeSerialPort();

	return 0;
}

#endif /* APPLICATION_H_ */
