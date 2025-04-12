/* request.c */

#include "request.h"
#include "router.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body);
void serve_static_file(int client_fd, const char *filepath);

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
        // if (strcmp(method, "GET") == 0) {
        //         if (strcmp(path, "/") == 0) {
        //                 serve_static_file(client_fd, "/index.html");
        //         } else {
        //                 send_response(
        //                     client_fd, "200 OK", "text/html",
        //                     "<html><h1>Hello, World!</h1></html>\r\n");
        //         }
        // }

        // Close socket
        close(client_fd);
}
