/* server.c */

#include "request.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

void *client_thread(void *arg) {
        int client_fd = *(int *)arg;
        free(arg);

        handle_request(client_fd);

        // Terminate the thread
        pthread_exit(NULL);
        return NULL;
}

int start_server() {
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
                if (*client_fd < 0) {
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
