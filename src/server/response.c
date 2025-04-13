/* response.c */
#include "response.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BIG_BUFFER 8192

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body) {
        printf(" >> SEND_RESPONSE()\n");
        char response[BIG_BUFFER];
        int length = snprintf(response, sizeof(response),
                              "HTTP/1.1 %s\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zu\r\n"
                              "\r\n%s",
                              status, content_type, strlen(body), body);

        // Write to the socket
        write(client_fd, response, length);
}

void send_response_binary(int client_fd, const char *status, const char *type,
                          const char *body, size_t length) {
        char header[512];
        int header_len = snprintf(header, sizeof(header),
                                  "HTTP/1.1 %s\r\n"
                                  "Content-Type: %s\r\n"
                                  "Content-Length: %zu\r\n"
                                  "Connection: close\r\n"
                                  "\r\n",
                                  status, type, length);

        write(client_fd, header, header_len);
        write(client_fd, body, length);
}
