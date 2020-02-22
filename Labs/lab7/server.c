/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    char buf[BUFLEN];
    struct sockaddr_in server_addr;
    int len;
    int res;
    int sock_fd;
    int yes = 1;



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

    server_addr.sin_family = AF_INET;                   // de la IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //sin_addr este o structura care are campul s_addr (pag 4)
    server_addr.sin_port = htons(atoi(argv[2]));


    // legare proprietati de socket

    len = sizeof(server_addr);
    int status = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    if (status < 0)
    {
        perror("Socket Binding Failed");
        exit(EXIT_FAILURE);
    }

    // ascultare de conexiuni

    res = listen(sock_fd, 1);

    if (res < 0)
    {
        perror("Listening Failed");
        exit(EXIT_FAILURE);
    }

    // acceptarea unei conexiuni, primirea datelor, trimiterea raspunsului

    int connect_sock = accept(sock_fd, (struct sock_addr*) &server_addr, &len);

    if (connect_sock < 0) {
        puts("Server acccept failed...\n");
        exit(0);
    }

    while(1) {

        res = recv(connect_sock, buf, BUFLEN, 0);

        if (res < 0)
        {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        } else if (res == 0) {
            puts("Server closing...");
            close(sock_fd);
            exit(0);
        }

        printf("%s\n", buf);

        send(connect_sock, buf, BUFLEN, 0);
    }

    // inchidere socket(i)

}
