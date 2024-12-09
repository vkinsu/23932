#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
	for (int i = 0; str[i]; i++) {
		str[i] = toupper((unsigned char)str[i]);
	}
}

int main() {
	int server_fd, client_fd;
	struct sockaddr_un server_addr;
	char buffer[BUFFER_SIZE];

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

	if (listen(server_fd, 5) == -1) {
		perror("Listen failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Server listening on %s\n", SOCKET_PATH);

	if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
		perror("Accept failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	while (1) {
		memset(buffer, 0, BUFFER_SIZE);
		ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
		if (bytes_read > 0) {
			to_uppercase(buffer);
			printf("Received and converted: %s\n", buffer);
		} else if (bytes_read == 0) {
			printf("Client disconnected\n");
			break;
		} else {
			perror("Read failed");
			break;
		}
	}

	close(client_fd);
	close(server_fd);
	unlink(SOCKET_PATH);

	return 0;
}

