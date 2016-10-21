#ifndef ALARM_H
#define ALARM_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "Utils.h"

unsigned char *COMMAND, *FRAME;
int triesSend = 0, triesConnect = 0;

void connect() {
	if (triesConnect < dl->retries) {
		printf("No response. Tries left = %d\n", dl->retries - triesConnect);
		write(app->fd, COMMAND, COMMAND_SIZE);
		triesConnect++;
		alarm(dl->timeout);
	} else {
		alarm(0);
		printf("ERROR: Failed to create a connection.\n");
		exit(-1);
	}
}

void send() {
	if (triesSend < dl->retries) {
		printf("No response. Tries left = %d\n", dl->retries - triesSend);
		write(app->fd, FRAME, FRAME_SIZE);
		triesSend++;
		alarm(dl->timeout);
	} else {
		alarm(0);
		printf("ERROR: Failed to send frame.\n");
		exit(-1);
	}
}

#endif