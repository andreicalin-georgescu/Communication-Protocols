/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * client mini-server de backup fisiere
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

void usage(char *file)
{
	fprintf(stderr, "Usage: %s ip_server port_server file\n", file);
	exit(0);
}

/*
 * Utilizare: ./client ip_server port_server nume_fisier_trimis
 */
int main(int argc, char **argv)
{
	int sock_fd;
	socklen_t addrlen;
	struct sockaddr_in server_addr;
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

	server_addr.sin_family = AF_INET;// de la IPv4
	server_addr.sin_port = htons(PORT);

	status = inet_aton("127.0.0.1", &server_addr.sin_addr); // setez ip-ul serverului
	if (status <= 0)
	{
		perror("inet_aton");
		exit(-1);
	}

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	addrlen = sizeof(server_addr);

	char *bufFromClient = "De la client pentru server, adevarat!";
	sendto(sock_fd, bufFromClient, strlen(bufFromClient), 0, (struct sockaddr *)&server_addr, addrlen);
	puts("Message sent");

	ret = recvfrom(sock_fd, (char *)buf, sizeof(buf) - 1, 0, (struct sockaddr *)&server_addr, &addrlen);
	buf[ret] = '\0';
	puts("Serverul mi-a trimis");
	puts(buf);

	close(sock_fd);
	return 0;
}
