#include <stdio.h>

int main() {
	int i, ns = 0;

	for (i = 0; i < 100; i++) {
		unsigned char byte = ns << 6;
		printf("n = %d, 0x%02x\n", ns++, byte);
	}

	return 0;
}