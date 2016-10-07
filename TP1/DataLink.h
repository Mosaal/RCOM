#ifndef DATALINK_H_
#define DATALINK_H_

#include <stdlib.h>
#include <termios.h>
#include "Utils.h"
#include "Application.h"

typedef struct {
	char *port;
	char frame[MAX_SIZE];
	int mode, baudrate, max_size;
	struct termios oldtio, newtio;
	unsigned int ns, timeout, retries;
} DataLink;

typedef struct {
	char *file, *port;
	int fd, mode;
} Application;

int writeFrame(Application *app, char *sframe) {

  if(write(app->fd, sframe, 5) == -1) {
		printf("ERROR: Failed to write frame.\n");
		return -1;
	}

	return 0;
}

int readFrame(Application *app, char *frame) {
	char chr;
	int j;

	if(read(app->fd, &chr, 1) == -1 || chr != FLAG) {
		printf("ERROR: Failed to read frame.\n");
		return -1;
	}
	else {
		for(j = 1; j < 5; j++)
		{
			read(app->fd, &chr, 1);
      if(chr != frame[j])
      {
        printf("ERROR: Failed recieving byte %d of frame\n", j);
        return -1;
      }
		}
	}
	return 0;
}

int initDataLink(Application *app, DataLink *dl) {
	dl->ns = 0;
	dl->mode = app->mode;
	dl->port = app->port;
	dl->timeout = TIMEOUT;
	dl->retries = RETRIES;
	dl->baudrate = BAUDRATE;
	dl->max_size = MAX_SIZE;

	// Save current port settings
	if (tcgetattr(app->fd, &dl->oldtio) == -1) {
		printf("ERROR: Could not save current port settings.\n");
		return -1;
	}

	// Set new termios structure
	bzero(&dl->newtio, sizeof(dl->newtio));
	dl->newtio.c_cflag = dl->baudrate | CS8 | CLOCAL | CREAD;
	dl->newtio.c_iflag = IGNPAR;
	dl->newtio.c_oflag = 0;
	dl->newtio.c_lflag = 0;
	if(app->mode == SEND)
	{
		dl->newtio.c_cc[VMIN] = 0;
		dl->newtio.c_cc[VTIME] = 30;
	}
	else if(app->mode == RECEIVE)
	{
		dl->newtio.c_cc[VMIN] = 5;
		dl->newtio.c_cc[VTIME] = 10;
	}
	else
		return(-1);
	tcflush(app->fd, TCIOFLUSH);
	if (tcsetattr(app->fd, TCSANOW, &dl->newtio) == -1) {
		printf("ERROR: Failed to set new termios structure.\n");
		return -1;
	}

	return 0;
}

int closeSerialPort(Application *app, DataLink *dl) {
	if (tcsetattr(app->fd, TCSANOW, &dl->oldtio) == -1) {
		perror("tcsetattr");
		return -1;
	}

	close(app->fd);
	return 0;
}

int llopen(Application *app) {
	if(app->mode == SEND) {
		unsigned char SET[5], UA[5];
		int i = 1;
		SET[0] = FLAG;
		SET[1] = A;
		SET[2] = C_SET;
		SET[3] = SET[1] ^ SET[2];
		SET[4] = FLAG;

		UA[0] = FLAG;
		UA[1] = A;
		UA[2] = C_UA;
		UA[3] = UA[1] ^ UA[2];
		UA[4] = FLAG;

		/* há aqui um problema quando ele esta a fazer o write e
		   read das tramas porque se ligares o receiver quando o
			 outro ainda esta a escrever da erro, basicamente se
			 lançares o receiver imediatamente a seguir a aparecer
			 o retrying to send frame ele nao funca*/

		while(i < 5) {
			if(writeFrame(app, SET) == -1) {
				printf("ERROR: Failed to write SET frame\n");
				return -1;
			}
			if(readFrame(app, UA) == -1) {
				if(i == 4)
					return -1;
				printf("       Retrying to send frame #%d\n", i);
				i++;
			}
			else {
				printf("Recieved UA frame\n");
				return 0;
			}
		}
		return -1;

	}
	else {
		unsigned char SET[5], UA[5];
		SET[0] = FLAG;
		SET[1] = A;
		SET[2] = C_SET;
		SET[3] = SET[1] ^ SET[2];
		SET[4] = FLAG;
		printf("Waiting to read SET frame\n");
		if(readFrame(app, SET) == -1) {
			return -1;
		}

		UA[0] = FLAG;
		UA[1] = A;
		UA[2] = C_UA;
		UA[3] = UA[1] ^ UA[2];
		UA[4] = FLAG;
		if(writeFrame(app, UA) == -1) {
			printf("ERROR: Failed to write UA frame\n");
			return -1;
		}
		printf("Recieved SET frame\n");
		return 0;
	}
}

int llread();

int llwrite();

int llclose();

#endif /* DATALINK_H_ */
