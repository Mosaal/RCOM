#ifndef APPLICATION_H
#define APPLICATION_H

#include <string.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Utils.h"
#include "DataLink.h"

int initApplication(int mode, char *port, char *fileName) {
	Application *app;
	app = (Application *)malloc(sizeof(Application));

	app->mode = mode;
	app->port = port;
	app->fileName = fileName;
	app->fd = open(port, O_RDWR | O_NOCTTY);

	if (app->fd < 0) {
		printf("ERROR: Failed to open serial port.\n");
		return -1;
	}

	DataLink *dl;
	initDataLink(app, dl);

	/*if (llopen(app->fd, app->mode) == -1) {
		printf("ERROR: Failed to create a connection.\n");
		return -1;
	}*/

	if (app->mode == SEND) {
		FILE *file;
		file = fopen(fileName, "rb");
		if (file == NULL) {
			printf("ERROR: Failed to open file \"%s\".\n", fileName);
			return -1;
		}

		unsigned char buffer[getFileSize(file)];
		size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
		fclose(file);

		if (llwrite(app->fd, buffer, MAX_SIZE) == -1) {
			printf("ERROR: Failed to send all of tha data.\n");
			return -1;
		}
	} else if (app->mode == RECEIVE) {

	}

	/*if (llclose(app->fd, app->mode) == -1) {
		printf("ERROR: Failed to close the connection.\n");
		return -1;
	}*/

	closeSerialPort(app, dl);
}

#endif