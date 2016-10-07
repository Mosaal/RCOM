#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "Utils.h"
#include "DataLink.h"

void printConfig(Application *app) {
	printf("CURRENT CONFIGURATIONS\n");
	if(app->mode == SEND)
		printf(" - Mode: SENDER\n");
	if(app->mode == RECEIVE)
		printf(" - Mode: RECEIVER\n");
	//printf(" - Baud rate: %d\n", ll->baudRate);
//	printf(" - Message data max. size: %d\n", ll->messageDataMaxSize);
//	printf(" - Max. no. retries: %d\n", ll->numTries - 1);
//	printf(" - Time-out interval: %d\n", ll->timeout);
	printf(" - Port: %s\n", app->port);
//	printf(" - File: %s\n", al->fileName);
}

int sendFile(Application *app) {
	/*FILE *file = fopen(app->file, "rb");
	if (file == NULL) {
		printf("ERROR: Failed to open \"%s\".", app->file);
		return -1;
	}*/
	llopen(app);

	return 0;
}

int receiveFile(Application *app) {
	llopen(app);
	return 0;
}

int initApplication(int mode, char *port, char *file) {
	Application *app;
	app = (Application *)malloc(sizeof(Application));

	app->mode = mode;
	app->file = file;
	app->fd = open(port, O_RDWR | O_NOCTTY);
	app->port = port;

	if (app->fd < 0) {
		printf("ERROR: Failed to open serial port.\n");
		return -1;
	}

	DataLink *dl;
	dl = (DataLink *)malloc(sizeof(DataLink));
	initDataLink(app, dl);

	printConfig(app);

	switch (app->mode) {
	case SEND:
		sendFile(app);
		break;
	case RECEIVE:
		receiveFile(app);
		break;
	}

	closeSerialPort(app, dl);

	return 0;
}

#endif /* APPLICATION_H_ */
