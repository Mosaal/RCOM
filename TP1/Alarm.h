#ifndef ALARM_H
#define ALARM_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "Utils.h"

int tries = 0;

void reconnect() {
	if (tries < dl->retries) {
		printf("Retrying to send SET buffer.\n");
		write(app->fd, SET, sizeof(SET));
		tries++;
		alarm(dl->timeout);
	} else {
		printf("Receiver not replying.\n");
		alarm(0);
		exit(-1);
	}
}

#endif