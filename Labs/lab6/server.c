/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * mini-server de backup fisiere
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "helpers.h"

#define PORT 8080


void usage(char*file)
{
	fprintf(stderr, "Usage: %s server_port file\n", file);
	exit(0);
}

/*
 * Utilizare: ./server server_port nume_fisier
 */
int main(int argc, char **argv)
{

	int sock_fd;
	socklen_t addrlen;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr; //detalii despre client
	char buf[BUFLEN];
	int ret;
	int status = 0;
	int yes = 1;

	/* TODO deschidere socket */
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	/* TODO setare struct sockaddr_in pentru a specifica unde trimit
 	* datele */

	server_addr.sin_family = AF_INET;                   // de la IPv4
	server_addr.sin_addr.s_addr = INADDR_ANY; //sin_addr este o structura care are campul s_addr (pag 4)
	server_addr.sin_port = htons(PORT);             // portul

	/* TODO legare proprietăți de socket */

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	status = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if (status < 0)
	{
		perror("Socket Binding Failed");
		exit(EXIT_FAILURE);
	}


	//partea de comunicare efectiva

	addrlen = sizeof(client_addr);
	puts("Waiting for message");
	// int recvfrom(int sockfd, char* buff, int nbytes, int flags, struct sockaddr *from, int addrlen);
	// primesc ceva de la client, pe socket-ul pe care l-am deschis
	ret = recvfrom(sock_fd, (char *)buf, sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &addrlen);
	buf[ret] = '\0';
	puts("Clientul mi-a trimis");
	puts(buf);

	char *bufFromServer = "De la server pentru client, adevarat!";

	// trimit inapoi la client, pe socket
	// int sendto(int sockfd, char* buff, int nbytes, int flags, struct sockaddr *to, int addrlen);

	sendto(sock_fd, bufFromServer, strlen(bufFromServer), 0, (struct sockaddr *)&client_addr, addrlen);
	puts("Message sent");
	close(sock_fd);
	return 0;
}

