/* response.c */

#include "response.h"
#include "mime.h"
#include "template.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

void send_response(int client_fd, const char *status, const char *content_type, const char *body) {
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

void send_response_binary(int client_fd, const char *status, const char *type, const char *body,
                          size_t length) {
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

void handle_static(int client_fd, const char *filepath) {
        printf(" >> HANDLE_STATIC(): %s\n", filepath);
        // sanitize file path to prevent directory traversal
        if (strstr(filepath, "..")) {
                send_response(client_fd, "403 Forbidden", "text/html", "<h1>403 - Forbidden</h1>");
                return;
        }

        char fullpath[SMALL_BUFFER] = "./src/web";
        strcat(fullpath, filepath);
        printf(" >> FILE_PATH: %s\n", filepath);
        printf(" >> FULL_PATH: %s\n", fullpath);

        serve_file(client_fd, fullpath);
}

void serve_file(int client_fd, const char *filepath) {
        const char *mime_type = get_mime_type(filepath);

        // Only process HTML files through template engine
        if (strcmp(mime_type, "text/html") == 0) {
                char *processed_content = process_template(filepath, NULL);
                if (!processed_content) {
                        send_response(client_fd, "500 Internal Server Error", "text/html",
                                      "<h1>Internal Server Error</h1>");
                        return;
                }

                size_t content_len = strlen(processed_content);

                send_response_binary(client_fd, "200 OK", mime_type, processed_content,
                                     content_len);
                free(processed_content);
        } else {
                FILE *file = fopen(filepath, "rb");
                if (!file) {
                        send_response(client_fd, "404 Not Found", "text/html",
                                      "<h1>404 - Not Found</h1>");
                        return;
                }
                file = fopen(filepath, "rb");

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
                        send_response(client_fd, "500 Internal Server Error", "text/html",
                                      "<h1>Internal Server Error</h1>");
                        return;
                }

                size_t bytesRead = fread(body, 1, size, file);
                fclose(file);

                if (bytesRead != size) {
                        free(body);
                        send_response(client_fd, "500 Internal Server Error", "text/html",
                                      "<h1>Internal Server Error</h1>");
                        return;
                }
                printf(" >> BYTESREAD: %zu\n", size);

                send_response_binary(client_fd, "200 OK", mime_type, body, size);
                free(body);
        }
}
