#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCK_PATH "socket_path"

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[1024];
    int bytes_received;

    // Создаем сокет
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Удаляем существующий сокет, если он есть
    unlink(SOCK_PATH);

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    // Привязываем сокет к адресу
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Слушаем на сокете
    if (listen(server_sock, 5) < 0) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Принимаем соединение от клиента
    client_len = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("accept");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Читаем данные от клиента
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        close(client_sock);
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_received] = '\0';

    // Переводим текст в верхний регистр
    for (int i = 0; i < bytes_received; i++) {
        buffer[i] = toupper(buffer[i]);
    }

    // Выводим результат
    printf("Received and converted text: %s\n", buffer);

    // Закрываем сокеты
    close(client_sock);
    close(server_sock);

    // Удаляем сокет
    unlink(SOCK_PATH);

    return 0;
}

