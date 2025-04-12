/* router.c */
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

#define GET "GET"

#define PUBLIC_ROUTE "/public/"

typedef struct {
        const char *method;
        const char *path;
        void (*handler)(int, const char *, const char *);
} Route;

void send_response(int client_fd, const char *status, const char *type,
                   const char *body);
void serve_file(int client_fd, const char *filepath);
void handle_root(int client_socket, const char *query, const char *body);
void handle_static(int client_socket, const char *path);

Route routes[] = {{"GET", "/", handle_root}};

void handle_route(int client_fd, const char *method, const char *path,
                  const char *query, const char *body) {
        printf(" >> HANDLE_ROUTE()\n");
        // handle any defined routes
        for (int i = 0; i < sizeof(routes) / sizeof(Route); i++) {
                if (strcmp(routes[i].method, method) == 0 &&
                    strcmp(routes[i].path, path) == 0) {
                        routes[i].handler(client_fd, query, body);
                        return;
                }
        }

        // handle static route
        if (strcmp(method, GET) == 0 &&
            strncmp(path, PUBLIC_ROUTE, strlen(PUBLIC_ROUTE)) == 0) {
                handle_static(client_fd, path);
                return;
        }

        // 404 not found
        // send_response
}

void serve_file(int client_fd, const char *filepath) {
        printf(" >> SERVE_FILE()\n");
        FILE *file = fopen(filepath, "r");
        if (!file) {
                send_response(client_fd, "404 Not Found", "text/html",
                              "<h1>404 - Not Found</h1>");
                return;
        }

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);

        char *body = malloc(size + 1);
        fread(body, 1, size, file);
        body[size] = '\0';

        send_response(client_fd, "200 OK", "text/html", body);
        fclose(file);
        free(body);
}

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

void handle_root(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_ROOT()\n");
        handle_static(client_fd, "/index.html");
}

void handle_static(int client_fd, const char *filepath) {
        printf(" >> HANDLE_STATIC()\n");
        // sanitize file path to prevent directory traversal
        if (strstr(filepath, "..")) {
                send_response(client_fd, "403 Forbidden", "text/html",
                              "<h1>403 - Forbidden</h1>");
                return;
        }
        char fullpath[SMALL_BUFFER] = "./src/web";
        strcat(fullpath, filepath);
        printf(" >> FILE_PATH: %s\n", filepath);
        printf(" >> FULL_PATH: %s\n", fullpath);

        FILE *file = fopen(fullpath, "r");
        if (!file) {
                send_response(client_fd, "404 Not Found", "text/html",
                              "<html><h1>404 - File Not Found</h1></html>\r\n");
                return;
        }

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);

        char *body = malloc(size + 1);
        fread(body, 1, size, file);
        body[size] = '\0';

        send_response(client_fd, "200 OK", "text/html", body);
        fclose(file);
        free(body);
}
