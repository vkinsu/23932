#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

void to_uppercase(char *str) {
	for (int i = 0; str[i]; i++) {
		str[i] = toupper((unsigned char)str[i]);
	}
}

int main() {
	int server_fd, client_fd, max_fd;
	int active_clients = 0;
	struct sockaddr_un server_addr;
	char buffer[BUFFER_SIZE];
	fd_set read_fds, active_fds;

	if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
	unlink(SOCKET_PATH);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("Bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, MAX_CLIENTS) == -1) {
		perror("Listen failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Server listening on %s\n", SOCKET_PATH);

	FD_ZERO(&active_fds);
	FD_SET(server_fd, &active_fds);
	max_fd = server_fd;

	while (1) {
        read_fds = active_fds;

	if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("Select failed");
		break;
	}

	for (int i = 0; i <= max_fd; i++) {
		if (FD_ISSET(i, &read_fds)) {
			if (i == server_fd) {
				if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
					perror("Accept failed");
					continue;
				}
				printf("New client connected: %d\n", client_fd);
				FD_SET(client_fd, &active_fds);
				if (client_fd > max_fd) max_fd = client_fd;
				active_clients++;
				} else {
					memset(buffer, 0, BUFFER_SIZE);
					ssize_t bytes_read = read(i, buffer, BUFFER_SIZE - 1);
					if (bytes_read > 0) {
						to_uppercase(buffer);
						printf("Client %d: %s\n", i, buffer);
					} else {
						if (bytes_read == 0) {
							printf("Client %d disconnected\n", i);
						} else {
							perror("Read failed");
						}
						close(i);
						FD_CLR(i, &active_fds);
						active_clients--;
					}
				}
			}
		}
		if (active_clients == 0) {
			printf("All clients disconnected. Server shutting down.\n");
			break;
		}
	}

	close(server_fd);
	unlink(SOCKET_PATH);
	return 0;
}

