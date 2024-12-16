#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, epoll_fd;
    struct sockaddr_un addr;
    struct epoll_event ev, events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH); // Удаляем старый файл сокета

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Failed to listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Создаем epoll-дескриптор
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("Failed to create epoll");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Добавляем серверный сокет в epoll
    ev.events = EPOLLIN;  // Готовность к чтению
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections...\n");

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);  // Ожидание событий
        if (nfds == -1) {
            perror("epoll_wait");
            close(server_fd);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                // Новое подключение
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("Failed to accept connection");
                    continue;
                }
                printf("New client connected: FD %d\n", client_fd);

                // Добавляем клиента в epoll
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    perror("epoll_ctl: client_fd");
                    close(client_fd);
                }
            }
            else {
                // Данные от клиента
                int client_fd = events[i].data.fd;
                ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

                if (bytes_read <= 0) {
                    if (bytes_read == 0) {
                        printf("Client disconnected: FD %d\n", client_fd);
                    }
                    else {
                        perror("Failed to read");
                    }
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                }
                else {
                    buffer[bytes_read] = '\0';
                    to_uppercase(buffer);
                    printf("Received from client %d: %s\n", client_fd, buffer);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    unlink(SOCKET_PATH);
    return 0;
}