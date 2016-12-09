#ifndef UTILS_H
#define UTILS_H

#define PORT         21
#define SIZE         256
#define PROGRESS_BAR 45
#define REGEXP       "ftp://([([A-Za-z0-9])*:([A-Za-z0-9])*@])*([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+"

#include <regex.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*void printProgressBar(float curr, float total) {
	float per = (100.0 * curr) / total;
	printf("Completed: %6.2f%% [", per);

	int i, pos = (per * PROGRESS_BAR) / 100.0;
	for (i = 0; i < PROGRESS_BAR; i++) {
		if (i <= pos)
			printf("=");
		else
			printf(" ");
	}

	printf("]\r");
	fflush(stdout);
}*/

#endif