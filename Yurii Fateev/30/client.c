#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    // Создание сокета
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Локальный адрес

    // Подключение к серверу
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Enter the text to send to the server (or 'exit' to complete):\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("> ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }

        // Удаляем символ перевода строки
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Отправка данных на сервер
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            break;
        }
    }

    close(client_fd);
    printf("The client has completed the work.\n");
    return 0;
}
