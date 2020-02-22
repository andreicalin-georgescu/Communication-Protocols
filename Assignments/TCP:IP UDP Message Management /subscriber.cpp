#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "helpers.h"

void usage(char *file)
{
    fprintf(stderr, "Usage: %s id_client server_address server_port \n", file);
    exit(0);
}

int main(int argc, char *argv[]) {

    int sockfd, n, ret, i;
    struct sockaddr_in serv_addr;
    char buffer[BUFLEN];

    if (argc < 4) {
        usage(argv[0]);
    }

    fd_set read_fds;	// multimea de citire folosita in select()
    fd_set tmp_fds;		// multime folosita temporar
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    int fdmax = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    ret = inet_aton(argv[2], &serv_addr.sin_addr);
    DIE(ret == 0, "inet_aton");

    ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    DIE(ret < 0, "connect");

    FD_SET(STDIN_FILENO, &read_fds);

    FD_SET(sockfd, &read_fds);

    if (sockfd > fdmax) {
        fdmax = sockfd;
    }

    // send client id
    strcpy(buffer, argv[1]);
    n = send(sockfd, buffer, strlen(buffer), 0);
    DIE(n < 0, "send");

    while (1) {
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == STDIN_FILENO) {
                    // se citeste de la tastatura
                    memset(buffer, 0, BUFLEN);
                    fgets(buffer, BUFLEN - 1, stdin);

                    if (strncmp(buffer, "exit", 4) == 0) {
                        FD_CLR(i, &read_fds);
                        FD_CLR(i, &tmp_fds);
                        FD_CLR(sockfd, &read_fds);
                        FD_CLR(sockfd, &tmp_fds);
                        close(sockfd);
                        return 0;
                    }

                    // se trimite mesaj la server
                    n = send(sockfd, buffer, strlen(buffer), 0);
                    DIE(n < 0, "send");
                } else if (i == sockfd){
                    // primesc mesaj de la server
                    memset(buffer, 0, BUFLEN);
                    n = recv(sockfd, buffer, sizeof(buffer), 0);
                    DIE(n < 0, "recv");

                    // daca primesc mesaj de exit, inchid si clientul
                    if(strncmp(buffer, "exit", 4) == 0) {
                        close(sockfd);
                        return 0;
                    }
                    printf("%s\n", buffer);
                }
            }
        }
    }
}