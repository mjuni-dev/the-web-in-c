#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
// #define BUFFER_SIZE ULONG_MAX

int main() {
	int 			server_sock;
	socklen_t 		addrlen;
	struct sockaddr_in	host_addr;
	char buffer[BUFFER_SIZE];
	char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";

	// SOCKET -> BIND -> LISTEN -> ACCEPT -> READ/WRITE
	printf("\n >> starting server...\n\n");

	// Create the socket
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0) {
		perror("socket()\n");
		return -1;
	}
	printf(" >> socket created; fd: %i\n", server_sock);

	// Create the address we will bind the socket to
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind the socket to the address
	if (bind(server_sock, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0 ) {
		perror("bind()");
		return -1;
	}
	printf(" >> socket bound\n");

	// Listen for incoming connections
	if (listen(server_sock, 0) < 0) {
		perror("listen()");
		return -1;
	}
	printf(" >> socket listening\n");

	for (;;) {
		// struct sockaddr_in 	client_addr;
		// socklen_t 		client_addr_len;
		// int 			*client_fd;

		// client_addr_len = sizeof(client_addr);
		// client_fd = malloc(sizeof(int));

		// Accept incoming connections
		int c = accept(server_sock, (struct sockaddr *)&host_addr, &addrlen);
		if (c < 0) {
			perror("accept()");
			close(c);
			continue;
		}
		printf(" >> connection accepted; fd: %i\n", c);
		
		// Read from the socket
		int val = read(c, buffer, BUFFER_SIZE);
		if (val < 0) {
			perror("read()");
			close(c);
			continue;
		}
		
		// Write to the socket
		int w = write(c, resp, strlen(resp));
		if (w < 0) {
			perror("write()");
			close(c);
			continue;
		}
		// printf("\n%c\n", buffer);

		close(c);
	}

	printf("\n >> ...closing socket\n\n");
	close(server_sock);
	return 0;
}
