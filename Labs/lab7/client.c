/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int sock_fd = 0;
    char buf[BUFLEN], buf2[BUFLEN];
    struct sockaddr_in server_addr;
    int res;

    if(argc != 3)
    {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // completare informatii despre adresa serverului

    server_addr.sin_family = AF_INET;// de la IPv4
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &server_addr.sin_addr); // setez ip-ul serverului


    // conectare la server

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // citire de la tastatura, trimitere de cereri catre server, asteptare raspuns

    while(1) {
        //read from keybrd
        scanf("%s", buf);

        res = send(sock_fd, &buf, BUFLEN, 0);

        if(res < 0) {
            puts("Client sending failed\n");
            exit(EXIT_FAILURE);
        }

        res = recv(sock_fd, &buf2, BUFLEN, 0);

        if (!strcmp(buf, buf2)) {
            puts("Message sent ok\n");
        }

        if (!strcmp(buf, "exit")) {
            puts("Exiting...\n");

            // inchidere socket
            close(sock_fd);
            exit(0);
        }
    }



}
