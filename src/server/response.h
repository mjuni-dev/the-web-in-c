/* response.h */
#include <stdlib.h>

#ifndef RESPONSE_H
#define RESPONSE_H

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body);
void send_response_binary(int client_fd, const char *status, const char *content_type,
                   const char *body, size_t length);

#endif
