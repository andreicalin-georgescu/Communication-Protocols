#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char* get_cookie(char* response) {
    char* cookies = (char*)malloc(LINELEN);
    strcpy(cookies, "");
    strcat(cookies, "Cookie: ");

    // delimitatoare folosite

    char delim[] = " ";
    char* token = strtok(response, delim);
    // construire sir de caractere pentru cookie


    while(token !=  NULL) {
        if (strstr(token, "enunt") != NULL) {
            break;
        }
        if(strstr(token, "prajiturica") != NULL) {
            strcat(cookies, token);
        }
        token = strtok(NULL, delim);
    }
    return cookies;

}

int main(int argc, char *argv[])
{
    //int i;
    char *message;
    char *token;
    char *response;
    int sockfd;

    JSON_Value *root_value;
    JSON_Object *root_obj;

    // TODO Task 1
    // trimitem serverului mesajul cu formatul cerut

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, "/task1/start", NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);

    //TODO Task 2

    // parsam incepand de la campul body

    token = strchr(response, '{');
    root_value = json_parse_string(token);


    root_obj = json_value_get_object(root_value);
    // declarari necesare pentru task 2:

    const char *url_task2;
    const char *user;
    const char *pass;
    char *credentials = malloc(700);
    const char *content_type_task2;
    // extragem campurile dorite

    url_task2 = json_object_get_string(root_obj, "url");
    user = json_object_dotget_string(root_obj, "data.username");
    pass = json_object_dotget_string(root_obj, "data.password");
    content_type_task2 = json_object_get_string(root_obj, "type");

    // construim datele de autentificare

    sprintf(credentials, "username=%s&password=%s", user, pass);
    // parsam cookieurile

    char* cookie_task2 = get_cookie(response);
    // trimitem la server mesajul cu formatul cerut

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP_SERVER, (char *)url_task2, credentials, cookie_task2, NULL, content_type_task2);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // cleanup: inchidem conexiunea cu serverul

    close_connection(sockfd);
    // TODO Task 3

    // parsam incepand de la campul body

    token = strchr(response, '{');
    root_value = json_parse_string(token);


    root_obj = json_value_get_object(root_value);
    // declarari necesare pentru task 3

    const char *id;
    char *url_params3 = malloc(LINELEN);
    const char *url_task3;
    const char *auth_token;

    url_task3 = json_object_get_string(root_obj, "url");

    strcpy(url_params3, "");
    // creare url nou care contine raspunsurile intrebarilor si id-ul parsat

    strcat(url_params3, "raspuns1=omul&raspuns2=numele&id=");
    id = json_object_dotget_string(root_obj, "data.queryParams.id");
    strcat(url_params3, id);

    // creare authentication token
    auth_token = json_object_dotget_string(root_obj, "data.token");
    // parsam cookieurile

    char* cookie_task3 = get_cookie(response);
    // trimitem mesajul cerut la server

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request_task3(IP_SERVER, (char*)url_task3, url_params3, cookie_task3, auth_token);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // cleanup: inchidem conexiunea cu serverul

    close_connection(sockfd);

    // TODO Task 4
    // parsam incepand de la campul body

    token = strchr(response, '{');
    root_value = json_parse_string(token);

    root_obj = json_value_get_object(root_value);
    // declarari necesare pentru task 4

    const char *url_task4;
    url_task4 = json_object_get_string(root_obj, "url");
    // parsam cookieurile

    char *cookie_task4 = get_cookie(response);
    // trimitem mesajul cerut la server, de data asta fara lista de parametrii si folosind jwt-ul primit anterior

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request_task3(IP_SERVER, (char*)url_task4, NULL, cookie_task4, auth_token);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // cleanup: eliberam memoria alocata pe heap si inchidem conexiunea cu serverul

    close_connection(sockfd);
    free(cookie_task4);
    free(cookie_task3);
    free(url_params3);
    free(cookie_task2);
    free(credentials);

    //TODO Task 5
    //parsam incepand de la campul body

    token = strchr(response, '{');
    root_value = json_parse_string(token);
    root_obj = json_value_get_object(root_value);
    // declarari necesare pentru task 4

    const char *url_task5;
    const char *request_url;
    char *url_params5 = malloc(LINELEN);
    const char *param_q;
    const char *param_appid;
    const char *weather_ip;
    const char *content_type_task5;
    // construim campurile necesare pentru interogarea celor doua servere

    url_task5 = json_object_get_string(root_obj, "url");
    request_url = json_object_dotget_string(root_obj, "data.url");
    param_q = json_object_dotget_string(root_obj, "data.queryParams.q");
    param_appid = json_object_dotget_string(root_obj, "data.queryParams.APPID");
    content_type_task5 = json_object_get_string(root_obj, "type");

    sprintf(url_params5, "q=%s&APPID=%s", param_q, param_appid);
    //extragem cookieurile din raspunsukl primit de la server

    char *cookies_task5 = get_cookie(response);
    // aflam adresa ip a serverului de vreme

    char delim_task5[] = "/";
    char *request_url_copy = malloc(LINELEN);
    strcpy(request_url_copy, request_url);

    token = strtok(request_url_copy, delim_task5);
    int ret;
    struct addrinfo hints, *result;
    // setam hinturile

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP;
    // extragem adresa

    ret = getaddrinfo(token, NULL, &hints, &result);
    if (ret != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in* addr = (struct sockaddr_in*) result->ai_addr;
    addr->sin_port = htons(80);
    inet_ntop(result->ai_family, &(addr->sin_addr), ip, sizeof(ip));

    weather_ip = ip;
    // cream requestul pe care il trimitem la serverul de weather

    sockfd = open_connection((char*)weather_ip, 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request((char*)weather_ip, strchr(request_url, '/'), url_params5);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // eliberam memoria alocata si inchidem conexiunea cu serverul de weather

    close_connection(sockfd);
    free(request_url_copy);
    free(url_params5);
    // parsam raspunsul serverului si extragem datele in tip json

    token = strchr(response, '{');
    // trimitem raspunsul catre server in formatul cerut

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP_SERVER, (char*)url_task5, token, cookies_task5, auth_token, content_type_task5);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // eliberam memoria alocata si inchidem conexiunea cu serverul

    close_connection(sockfd);
    free(cookies_task5);
    // eliberam memoria alocata pentru constructia mesajului si raspunsurilor de la server

    free(message);
    free(response);
    return 0;
}