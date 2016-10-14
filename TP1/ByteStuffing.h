#ifndef BYTESTUFFING_H
#define BYTESTUFFING_H

#include <stdlib.h>
#include <string.h>
#include "Utils.h"

int stuff(unsigned char **buf, int length) {
	int i, newLength = length;

	for (i = 1; i < length - 1; i++) {
		if ((*buf)[i] == FLAG || (*buf)[i] == ESCAPE)
			newLength++;
	}

	*buf = (unsigned char *)realloc(*buf, newLength);

	for (i = 1; i < length - 1; i++) {
		if ((*buf)[i] == FLAG || (*buf)[i] == ESCAPE) {
			memmove(*buf + i + 1, *buf + i, length - 1);
			length++;
			(*buf)[i] = ESCAPE;
			(*buf)[i + 1] ^= 0x20;
		}
	}

	return newLength;
}

int destuff(unsigned char **buf, int length) {
	int i;
	for (i = 1; i < length - 1; i++) {
		if ((*buf)[i] == ESCAPE) {
			memmove(*buf + i, *buf + i + 1, length - i - 1);
			length--;
			(*buf)[i] ^= 0x20;
		}
	}

	*buf = (unsigned char *)realloc(*buf, length);

	return length;
}

#endif