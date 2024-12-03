#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

int main() {
	int sock_fd;
	struct sockaddr_un server_addr;
	char buffer[BUFFER_SIZE];

	if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("Connect failed");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	printf("Connected to server. Enter text (Ctrl+C to exit):\n");
		while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
			if (write(sock_fd, buffer, strlen(buffer)) == -1) {
				perror("Write failed");
				break;
			}
		}

	close(sock_fd);
	return 0;
}

