/* response.h */
#include <stdlib.h>

#ifndef RESPONSE_H
#define RESPONSE_H

void serve_file(int client_fd, const char *filepath);

void handle_static(int client_socket, const char *path);

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body);

void send_response_binary(int client_fd, const char *status, const char *content_type,
                   const char *body, size_t length);

#endif
