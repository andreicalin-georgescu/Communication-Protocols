#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request_task3(char *host, char *url, char *url_params, char *cookies, const char *auth_token);
char *compute_post_request(char *host, char *url, char *form_data, char *cookies, const char *auth_token, const char *content_type);
char *compute_get_request(char *host, char *url, char *url_params);

#endif