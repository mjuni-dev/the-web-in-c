/* request.c */

#include "request.h"
#include "radix_tree_router.h"
#include "response.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

#define GET "GET"
#define POST "POST"

#define PUBLIC_ROUTE "/public/"
#define FAVICON "/favicon.ico"

void handle_request(int client_fd) {
        char buffer[BIG_BUFFER] = {0};
        char method[SMALL_BUFFER] = {0}, path[SMALL_BUFFER] = {0},
             protocol[SMALL_BUFFER] = {0};

        // memset(buffer, 0, sizeof(buffer));

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

        RouteHandler handler = find_route(method, path);
        if (handler) {
                handler(client_fd, query, body);
                send_response(client_fd, "200 OK", "text/html",
                              "<h1>Handler found</h1>");
                return;
        }

        // handle static route
        if (strcmp(method, GET) == 0 &&
            strncmp(path, PUBLIC_ROUTE, strlen(PUBLIC_ROUTE)) == 0) {
                handle_static(client_fd, path);
                return;
        }

        // handle favicon
        if (strcmp(method, GET) == 0 &&
            strncmp(path, FAVICON, strlen(FAVICON)) == 0) {
                handle_static(client_fd, "/public/img/favicon.ico");
                return;
        }

        // Routing
        // handle_route(client_fd, method, path, query, body);

        // Close socket
        close(client_fd);
}
