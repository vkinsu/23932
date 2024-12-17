#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/select.h>

#define SOCK_PATH "socket_path"
#define MAX_CLIENTS 10

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[1024];
    int bytes_received;
    fd_set read_fds;
    int max_fd;
    int client_fds[MAX_CLIENTS];
    int num_clients = 0;

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

    // Инициализируем массив клиентских сокетов
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }

    while (1) {
        // Инициализируем множество файловых дескрипторов
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        max_fd = server_sock;

        // Добавляем клиентские сокеты в множество
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != -1) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) {
                    max_fd = client_fds[i];
                }
            }
        }

        // Ждем активности на сокетах
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        // Проверяем активность на серверном сокете
        if (FD_ISSET(server_sock, &read_fds)) {
            client_len = sizeof(client_addr);
            client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("accept");
                close(server_sock);
                exit(EXIT_FAILURE);
            }

            // Добавляем новый клиентский сокет в массив
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == -1) {
                    client_fds[i] = client_sock;
                    num_clients++;
                    break;
                }
            }
        }

        // Проверяем активность на клиентских сокетах
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != -1 && FD_ISSET(client_fds[i], &read_fds)) {
                bytes_received = recv(client_fds[i], buffer, sizeof(buffer) - 1, 0);
                if (bytes_received <= 0) {
                    // Клиент отключился
                    close(client_fds[i]);
                    client_fds[i] = -1;
                    num_clients--;
                } else {
                    buffer[bytes_received] = '\0';
                    // Переводим текст в верхний регистр
                    for (int j = 0; j < bytes_received; j++) {
                        buffer[j] = toupper(buffer[j]);
                    }
                    // Выводим результат
                    printf("Received and converted text: %s\n", buffer);
                }
            }
        }
    }

    // Закрываем сокеты
    close(server_sock);

    // Удаляем сокет
    unlink(SOCK_PATH);

    return 0;
}

