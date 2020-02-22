#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *url_params)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // Scriem numele metodei, calea, parametrii din url (daca exista) si tipul protocolului

    if (url_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);
    // PAS 2: Adaugam host-ul

    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // PAS 3: Adaugam linia de final

    compute_message(message, "");
    free(line);
    return message;
}

char *compute_get_request_task3(char *host, char *url, char *url_params, char *cookies, const char* auth_token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // Scriem numele metodei, calea, parametri din url (daca exista) si tipul protocolului

    if (url_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);
    // PAS 2: Adaugam host-ul

    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // PAS 3: Adaugam headere si/ sau cookies, respectand forma protocolului

    sprintf(line, cookies);
    compute_message(message, line);
    // PAS 4: Adaugam Authentication headerul

    sprintf(line, "Authorization: Bearer %s", auth_token);
    compute_message(message, line);
    // PAS 5: Adaugam linia de final

    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *form_data, char *cookies, const char *auth_token,
        const char *content_type)
{

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // PAS 1: Scriem numele metodei, calea si tipul protocolului

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    // PAS 2: Adaugam host-ul


    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // PAS 3: Adaugam headere si/ sau cookies, respectand forma protocolului

    if(auth_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth_token);
        compute_message(message, line);
    }

    sprintf(line, cookies);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %d", (int)strlen(form_data));
    compute_message(message, line);

    // PAS 4: Adaugam linia de final de antent

    compute_message(message, "");
    // PAS 5: Adaugam data

    sprintf(line, form_data);
    compute_message(message, line);
    free(line);
    return message;
}

