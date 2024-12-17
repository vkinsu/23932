#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to connect to the server.");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection with server established. Enter message. (Ctrl+D to exit):\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        if (write(sock_fd, buffer, strlen(buffer)) == -1) {
            perror("Failed to write.");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(sock_fd);
    return 0;
}