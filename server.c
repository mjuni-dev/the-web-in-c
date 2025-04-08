#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define SMALL_BUFFER 1024
#define BIG_BUFFER 8192

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body) {
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

void serve_static_file(int client_fd, const char *filepath) {
        char fullpath[SMALL_BUFFER] = ".";
        strcat(fullpath, filepath);

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

void handle_request(int client_fd) {
        char buffer[BIG_BUFFER], method[SMALL_BUFFER], path[SMALL_BUFFER],
            protocol[SMALL_BUFFER];

        memset(buffer, 0, sizeof(buffer));

        // Read from the socket
        read(client_fd, buffer, sizeof(buffer) - 1);
        printf("REQUEST:\n%s\n", buffer);

        sscanf(buffer, "%s %s %s", method, path, protocol);
        printf("METHOD: %s\nPATH: %s\nPROTOCOL: %s\n", method, path, protocol);

        // check for query string params
        char *query_str = strchr(path, '?');
        char query[SMALL_BUFFER] = {0};
        if (query_str) {
                strcpy(query, query_str + 1);
                printf(" >> query_str: %s\n", query);
        }

        // Routing
        if (strcmp(method, "GET") == 0) {
                if (strcmp(path, "/") == 0) {
                        serve_static_file(client_fd, "/index.html");
                } else {
                        send_response(
                            client_fd, "200 OK", "text/html",
                            "<html><h1>Hello, World!</h1></html>\r\n");
                }
        }

        // Close socket
        close(client_fd);
}

void *client_thread(void *arg) {
        int client_fd = *(int *)arg;
        free(arg);

        handle_request(client_fd);

        // Terminate the thread
        pthread_exit(NULL);
        return NULL;
}

int main() {
        int server_fd;
        socklen_t addrlen;
        struct sockaddr_in host_addr;

        // SOCKET -> BIND -> LISTEN -> ACCEPT -> READ -> WRITE
        printf("\n >> starting server...\n\n");

        // Create the socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
                perror("socket()\n");
                return -1;
        }
        printf(" >> socket created; fd: %i\n", server_fd);

        // Create the address we will bind the socket to
        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PORT);
        host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        // Bind the socket to the address
        if (bind(server_fd, (struct sockaddr *)&host_addr, sizeof(host_addr)) <
            0) {
                perror("bind()");
                return -1;
        }
        printf(" >> socket bound\n");

        // Listen for incoming connections
        if (listen(server_fd, 0) < 0) {
                perror("listen()");
                return -1;
        }
        printf(" >> listening on http://localhost:%d\n", PORT);

        for (;;) {
                int *client_fd;

                client_fd = malloc(sizeof(int));

                // Accept incoming connections
                *client_fd =
                    accept(server_fd, (struct sockaddr *)&host_addr, &addrlen);
                if (client_fd < 0) {
                        perror("accept()");
                        close(*(int *)client_fd);
                        continue;
                }
                printf("\n\n >>>> CONNECTION ACCEPTED; CLIENT_FD: %i <<<<\n",
                       *(int *)client_fd);

                // Spawn a new thread to handle the request
                pthread_t thread_id;
                pthread_create(&thread_id, NULL, client_thread, client_fd);

                // Mark thread as detached
                pthread_detach(thread_id);
        }

        printf("\n >> ...closing socket\n\n");
        close(server_fd);
        return 0;
}
