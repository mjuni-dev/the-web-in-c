/* router.c */
#include "router.h"
#include "mime.h"
#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

#define GET "GET"
#define POST "POST"

#define PUBLIC_ROUTE "/public/"
#define FAVICON "/favicon.ico"

typedef struct {
        const char *method;
        const char *path;
        void (*handler)(int, const char *, const char *);
} Route;

void serve_file(int client_fd, const char *filepath);
void handle_get_root(int client_fd, const char *query, const char *body);
void handle_get_signin(int client_fd, const char *query, const char *body);
void handle_post_signin(int client_fd, const char *query, const char *body);
void handle_static(int client_socket, const char *path);

Route routes[] = {{GET, "/", handle_get_root},
                  {GET, "/auth/signin", handle_get_signin},
                  {POST, "/auth/signin", handle_post_signin}};

void handle_route(int client_fd, const char *method, const char *path,
                  const char *query, const char *body) {
        printf(" >> HANDLE_ROUTE()\n");
        // handle any defined routes
        for (int i = 0; i < (int)(sizeof(routes) / sizeof(Route)); i++) {
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

        // handle favicon
        if (strcmp(method, GET) == 0 &&
            strncmp(path, FAVICON, strlen(FAVICON)) == 0) {
                handle_static(client_fd, "/public/img/favicon.ico");
                return;
        }

        // 404 not found
        // send_response
}

void serve_file(int client_fd, const char *filepath) {
        printf(" >> SERVE_FILE()\n");
        FILE *file = fopen(filepath, "rb");
        if (!file) {
                send_response(client_fd, "404 Not Found", "text/html",
                              "<h1>404 - Not Found</h1>");
                return;
        }

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);
        printf(" >> SIZE: %zu\n", size);

        if (size == 0) {
                fclose(file);
                send_response(client_fd, "204 No Content", "text/html",
                              "<h1>No Content</h1>");
                return;
        }

        char *body = malloc(size + 1);
        if (!body) {
                fclose(file);
                send_response(client_fd, "500 Internal Server Error",
                              "text/html", "<h1>Internal Server Error</h1>");
                return;
        }

        size_t bytesRead = fread(body, 1, size, file);
        fclose(file);
        // body[size] = '\0';

        if (bytesRead != size) {
                free(body);
                send_response(client_fd, "500 Internal Server Error",
                              "text/html", "<h1>Internal Server Error</h1>");
                return;
        }
        printf(" >> BYTESREAD: %zu\n", size);

        const char *mime_type = get_mime_type(filepath);
        send_response_binary(client_fd, "200 OK", mime_type, body, size);
        // send_response(client_fd, "200 OK", mime_type, body);
        free(body);
}

void handle_post_signin(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_POST_SIGNIN()\n");

        // just removing warning from build for now.
        // Will probably change the signature, I don't know that I will need a
        // query for all handlers
        if (query) {
        }

        char username[128] = {0};
        char passwd[128] = {0};

        char *pair = strtok((char *)body, "&");
        while (pair) {
                printf(" >> PAIR: %s\n", pair);
                char key[128], val[128];
                if (sscanf(pair, "%127[^=]=%127s", key, val) == 2) {
                        if (strcmp(key, "username") == 0) {
                                strncpy(username, val, sizeof(username) - 1);
                        } else if (strcmp(key, "password") == 0) {
                                strncpy(passwd, val, sizeof(passwd) - 1);
                        }
                }
                pair = strtok(NULL, "&");
        }

        printf("Sign-in attemp: username: '%s', password: '%s'\n", username,
               passwd);

        char resp[512];
        snprintf(resp, sizeof(resp), "<h1>Welcome, %s!</h1>", username);
        send_response(client_fd, "200 OK", "text/html", resp);
}

void handle_get_signin(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_GET_SIGNIN()\n");

        // just removing warning from build for now.
        // I may change the signature.  I don't know if I will need a body for
        // get requests
        if (query && body) {
        }
        handle_static(client_fd, "/auth/signin.html");
}

void handle_get_root(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_GET_ROOT()\n");

        // just removing warning from build for now.
        // I may change the signature.  I don't know if I will need a body for
        // get requests
        if (query && body) {
        }
        handle_static(client_fd, "/index.html");
}

void handle_static(int client_fd, const char *filepath) {
        printf(" >> HANDLE_STATIC(): %s\n", filepath);
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

        serve_file(client_fd, fullpath);
}
