#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char *argv[])
{
	msg t;
	int i, res;
	
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);

	int w = atol(argv[1]) * 1000/ (sizeof(msg) * 8);

	printf("Window size: %d COUNT: %d\n", w, COUNT);


	if (w > COUNT) {
		w = COUNT;
	}

	//initial send
    int cnt = 1;

	for (i = 0; i < w; ++i) {

		/* send msg */
		res = send_message(&t);
		printf(" [%d] Sending initial messages\n", cnt);
		cnt++;
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}


	//wait for confirmation to send again
	for (i = 0; i < COUNT - w; i++) {
		/* wait for ACK */
		res = recv_message(&t);
		printf("[%d] Received ACK, sending again\n", cnt);
        cnt++;
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}

		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	cnt = 1;
	//wait for the final ACKs
	for (i = 0; i < w; ++i) {
		/* wait for ACK */
		printf("[%d] Waiting for last w ACKs\n", cnt);
		cnt++;
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
	}

	printf("[SENDER] Job done, all sent.\n");


	return 0;
}
