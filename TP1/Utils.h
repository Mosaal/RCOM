#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>

#define TRUE          1
#define FALSE         0

#define SEND          0
#define RECEIVE       1

#define RETRIES       3
#define TIMEOUT       3
#define MAX_SIZE      512
#define BAUDRATE      B38400
#define _POSIX_SOURCE 1

#define A             0x03
#define FLAG          0x7E
#define ESCAPE        0x7D

#define BIT(n)        (0x01 << n)

long int getFileSize(FILE *file) {
	if (fseek(file, 0, SEEK_END) == -1) {
		printf("ERROR: Could not get file size.\n");
		return -1;
	}

	long int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	return size;
}

#endif /* UTILS_H_ */
