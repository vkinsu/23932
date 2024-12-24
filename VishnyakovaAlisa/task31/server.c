#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 1024

void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds, temp_fds;
    int max_fd;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to bind.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Failed to listen.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    printf("Server started...\n");

    while (1) {
        temp_fds = read_fds;

        if (select(max_fd + 1, &temp_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                if (i == server_fd) {
                    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                        perror("Failed to accept connection");
                    }
                    else {
                        FD_SET(client_fd, &read_fds);
                        if (client_fd > max_fd) max_fd = client_fd;
                        printf("New client connected: FD %d\n", client_fd);
                    }
                }
                else {
                    ssize_t bytes_read = read(i, buffer, BUFFER_SIZE - 1);
                    if (bytes_read <= 0) {
                        if (bytes_read == 0) {
                            printf("Client disconnected: FD %d\n", i);
                        }
                        else {
                            perror("Failed to read");
                        }
                        close(i);
                        FD_CLR(i, &read_fds);
                    }
                    else {
                        buffer[bytes_read] = '\0';
                        to_uppercase(buffer);
                        printf("Received from client %d: %s\n", i, buffer);
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}