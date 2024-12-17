#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Создание сокета
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сокета
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Ожидание соединений
    if (listen(server_fd, 1) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("The server is waiting for a connection on the port %d\n", PORT);

    // Принятие соединения
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Завершаем строку
        to_uppercase(buffer);
        printf("Received and converted: %s\n", buffer);
    }

    if (bytes_read == -1) {
        perror("read");
    }

    close(client_fd);
    close(server_fd);
    printf("The server has shut down.\n");
    return 0;
}
