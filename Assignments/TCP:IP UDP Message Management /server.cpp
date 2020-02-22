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
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <vector>


#include "helpers.h"

void usage(char *file)
{
    fprintf(stderr, "Usage: %s <PORT_DORIT>\n", file);
    exit(0);
}

float changeSignFloat(const float a, message_t message) {
    if (message.content[0] == 1)
    {
        return a * (-1);
    }
    return a;
}

float normalize(const float a) {
    return a / 100.f;
}

int main(int argc, char *argv[]) {

    int sockfd_TCP, sockfd_UDP, newsockfd, portno, tmpsocket;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i, ret, j, yes = 1, status = 0;
    socklen_t clilen;
    socklen_t addrlen;
    char* token;
    message_t message;

    fd_set read_fds;
    fd_set tmp_fds;
    int fdmax = 0;
    // usage error

    if (argc < 2) {
        usage(argv[0]);
    }
    // cream asocierea intre clienti si topic si intre clienti si id

    std::map<std::string, std::vector<std::string> > topics;
    std::map<std::string, int> sockets;
    std::map<int, std::string> IDs;

    // vector pentru clientii conectati

    std::vector<int> connected_clients;
    // se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    // se adauga stdin la lista de read

    FD_SET(STDIN_FILENO, &read_fds);
    // socket TCP

    sockfd_TCP = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd_TCP < 0, "socket_TCP");
    // socket UDP

    sockfd_UDP = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(sockfd_UDP < 0, "socket_UDP");

    portno = atoi(argv[1]);
    DIE(portno == 0, "atoi");

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    // setare parametrii server

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // legare proprietati de socket pentru TCP

    ret = bind(sockfd_TCP, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
    DIE(ret < 0, "bind_TCP");

    // legare proprietati de socket pentru UDP

    if (setsockopt(sockfd_UDP, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    status = bind(sockfd_UDP, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    if (status < 0)
    {
        perror("Socket Binding Failed");
        exit(EXIT_FAILURE);
    }

    // listen pe portul de TCP

    ret = listen(sockfd_TCP, MAX_CLIENTS);
    DIE(ret < 0, "listen");
    // se adauga noii file descriptor (socketii pe care se asculta conexiuni) in multimea read_fds

    FD_SET(sockfd_TCP, &read_fds);
    FD_SET(sockfd_UDP, &read_fds);

    if (sockfd_TCP > fdmax) {
        fdmax = sockfd_TCP;
    }

    if (sockfd_UDP > fdmax) {
        fdmax = sockfd_UDP;
    }
    // bucla de executie

    while (1) {
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == sockfd_TCP) {
                    // a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
                    // pe care serverul o accepta

                    clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd_TCP, (struct sockaddr *) &cli_addr, &clilen);
                    DIE(newsockfd < 0, "accept");
                    // se adauga noul socket intors de accept() la multimea descriptorilor de citire

                    FD_SET(newsockfd, &read_fds);
                    if (newsockfd > fdmax) {
                        fdmax = newsockfd;
                    }

                    connected_clients.push_back(newsockfd);

                } else if (i == sockfd_UDP) {
                    // conexiune pe socketul de udp

                    addrlen = sizeof(cli_addr);
                    ret = recvfrom(sockfd_UDP, (char *)&message, sizeof(message), 0,
                                   (struct sockaddr *)&cli_addr, &addrlen);
                    DIE(ret < 0, "UDP receive error");
                    // vector de char pentru a construi mesajul

                    char buff[BUFLEN];
                    // stringstream pentru a transforma mesajul receptionat in formatul dorit

                    std::stringstream aux;

                    // construim prima parte a mesajului, comuna pentru toate tipurile de date primite

                    // punem portul in stream pentru a-l putea transforma in string
                    aux << ntohs(cli_addr.sin_port);

                    strcpy(buff, "");
                    strcat(buff, inet_ntoa(cli_addr.sin_addr));
                    strcat(buff, ":");
                    strcat(buff, aux.str().c_str());
                    strcat(buff, " - ");
                    strcat(buff, message.topic);
                    strcat(buff, " - ");

                    aux.str(std::string());


                    switch (message.data_type) {
                        // INT

                        case 0 : {
                            uint32_t continut = 0;

                            memcpy(&continut, message.content + 1, sizeof(continut));
                            continut = ntohl(continut);

                            if (message.content[0] != 0)
                            {
                                aux << (int)(-continut);
                            }
                            else
                            {
                                aux << continut;
                            }

                            strcat(buff, "INT");
                            strcat(buff, " - ");
                            strcat(buff, aux.str().c_str());
                            break;
                        }
                            // SHORT_REAL

                        case 1 : {
                            uint16_t continut = 0;
                            float continut_real;

                            memcpy(&continut, message.content, sizeof(continut));
                            continut = ntohs(continut);

                            continut_real = normalize(continut);

                            aux << std::fixed << std::setprecision(2) << continut_real;

                            strcat(buff, "SHORT_REAL");
                            strcat(buff, " - ");
                            strcat(buff, aux.str().c_str());
                            break;
                        }
                            // FLOAT

                        case 2 : {
                            uint32_t continut = 0;
                            uint8_t putere = 0;

                            float continut_real;

                            memcpy(&continut, message.content + 1, sizeof(continut));
                            continut = ntohl(continut);

                            memcpy(&putere, message.content + 1 + sizeof(continut), sizeof(putere));

                            continut_real = continut * pow(10, -putere);

                            continut_real = changeSignFloat(continut_real, message);

                            aux << std::fixed << std::setprecision(putere) << continut_real;

                            strcat(buff, "FLOAT");
                            strcat(buff, " - ");
                            strcat(buff, aux.str().c_str());
                            break;
                        }
                            // STRING

                        case 3 : {
                            strcat(buff, "STRING");
                            strcat(buff, " - ");
                            strcat(buff, message.content);
                            break;
                        }

                    }
                    // trimitere la abonati

                    for(int j = 0; j < topics[message.topic].size(); j++) {
                        n = send(sockets[topics[message.topic][j]], buff, strlen(buff), 0);
                        DIE(n < 0, "send");

                    }

                } else if(i == STDIN_FILENO) {
                    // se citeste din server de la tastatura
                    std::cin>>buffer;

                    if (strncmp(buffer, "exit", 4) == 0) {
                        // se inchid toti clientii

                        for (j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &read_fds)) {
                                // daca nu este socketul serverului si nu este tastatura
                                if (j != sockfd_TCP && j != STDIN_FILENO && j != sockfd_UDP) {
                                    n = send(j, buffer, strlen(buffer), 0);
                                    DIE(n < 0, "send");
                                }
                            }
                        }

                        // inchidem socketii descisi pentru comunicatie si incheiem executia programului
                        close(sockfd_TCP);
                        close(sockfd_UDP);
                        return 0;
                    }
                } else {
                    // s-au primit date pe unul din socketii de client TCP,
                    // asa ca serverul trebuie sa le receptioneze

                    for(int j = 0; j < connected_clients.size(); j++) {
                        if(i == connected_clients[j]) {
                            memset(buffer, 0, BUFLEN);
                            n = recv(i, buffer, sizeof(buffer), 0);
                            DIE(n < 0, "recv");

                            printf("New client (%s) connected from %s : %d\n",
                                   buffer, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

                            sockets[buffer] = i;
                            IDs[i] = buffer;

                            connected_clients.erase(connected_clients.begin() + j);
                        }
                    }
                    memset(buffer, 0, BUFLEN);
                    n = recv(i, buffer, sizeof(buffer), 0);
                    DIE(n < 0, "recv");

                    if (n == 0) {
                        // conexiunea s-a inchis

                        printf("Client (%s) disconnected\n", IDs[i].c_str());
                        close(i);

                        // se scoate din multimea de citire socketul inchis

                        FD_CLR(i, &read_fds);
                    } else {
                        // check subscribe/unsubscribe
                        token = strtok(buffer, " ");
                        if(strcmp(token, "subscribe") == 0) {
                            token = strtok(NULL, " \n");
                            // adaugam id-ul de tcp la lista de clienti abonati la topic

                            topics[token].push_back(IDs[i]);
                            token = strtok(NULL, " \n");


                        } else if(strcmp(token, "unsubscribe") == 0) {
                            token = strtok(NULL, " \n");
                            // stergem id-ul de tcp din lista de clienti abonati la topic
                            std::cout<<token;
                            for (int j = 0; j < topics[token].size(); j++) {
                                if(topics[token][j] == IDs[i]) {
                                    topics[token].erase(topics[token].begin() + j);
                                }
                            }

                        } else {
                            printf("Wrong command from client\n");
                        }

                    }
                }
            }
        }
    }







}

