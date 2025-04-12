/* request.c */

#include "request.h"
#include "router.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

void handle_request(int client_fd) {
        char buffer[BIG_BUFFER];
        char method[SMALL_BUFFER], path[SMALL_BUFFER], protocol[SMALL_BUFFER];

        memset(buffer, 0, sizeof(buffer));

        // Read from the socket
        read(client_fd, buffer, sizeof(buffer) - 1);
        printf("REQUEST:\n%s\n", buffer);

        sscanf(buffer, "%s %s %s", method, path, protocol);
        printf("METHOD: %s\nPATH: %s\nPROTOCOL: %s\n", method, path, protocol);

        // check for query string params
        char *query = strchr(path, '?');
        if (query) {
                // set '?' to null terminator
                *query = '\0';
                // move pointer forward one char
                query++;
                printf(" >> query: %s\n", query);
        } else {
                query = "";
        }

        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
                body += 4;
        } else {
                body = "";
        }

        // Routing
        handle_route(client_fd, method, path, query, body);

        // Close socket
        close(client_fd);
}
