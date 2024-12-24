#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example.sock"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Failed to connect to the server.");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection with server established. Enter message. (Ctrl+D to exit):\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    printf("Connection ended.\n");
    close(client_fd);

    return 0;
}