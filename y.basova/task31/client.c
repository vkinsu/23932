#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "socket_path"

int main() {
    int client_sock;
    struct sockaddr_un server_addr;
    char buffer[1024];

    // Создаем сокет
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    // Подключаемся к серверу
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    // Вводим текст для отправки
    printf("Enter text to send: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        perror("fgets");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    // Отправляем текст серверу
    if (send(client_sock, buffer, strlen(buffer), 0) < 0) {
        perror("send");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    // Закрываем сокет
    close(client_sock);

    return 0;
}

