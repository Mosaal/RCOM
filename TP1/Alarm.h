#ifndef ALARM_H
#define ALARM_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "Utils.h"

int triesConnect = 0;
unsigned char *COMMAND;

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

#endif