#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *client_thread(void *arg) {
	char buffer[BUFFER_SIZE];
	char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";
	int client_fd = *(int *)arg;
	free(arg);
	
	// Read from the socket
	int val = read(client_fd, buffer, BUFFER_SIZE);
	if (val < 0) {
		perror("read()");
		close(client_fd);
		pthread_exit(NULL);
		return NULL;
	}
	
	// Write to the socket
	int w = write(client_fd, resp, strlen(resp));
	if (w < 0) {
		perror("write()");
		close(client_fd);
		pthread_exit(NULL);
		return NULL;
	}

	close(client_fd);
	pthread_exit(NULL);
	return NULL;
}

int main() {
	int 			server_fd;
	socklen_t 		addrlen;
	struct sockaddr_in	host_addr;

	// SOCKET -> BIND -> LISTEN -> ACCEPT -> READ/WRITE
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
	if (bind(server_fd, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0 ) {
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
		int *client_fd, c;

		client_fd = malloc(sizeof(int));

		if (client_fd < 0) {
			perror("accept()");
			close(*(int *)client_fd);
			continue;
		}
		printf(" >> connection accepted; fd: %i\n", *(int *)client_fd);
		
		// Accept incoming connections
		*client_fd = accept(server_fd, (struct sockaddr *)&host_addr, &addrlen);

		pthread_t thread_id;
		pthread_create(&thread_id, NULL, client_thread, client_fd);
		pthread_detach(thread_id);
	}

	printf("\n >> ...closing socket\n\n");
	close(server_fd);
	return 0;
}
