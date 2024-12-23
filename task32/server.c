#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/epoll.h>

#define SOCKET_PATH "SOCKET"
#define MAX_EVENTS 10

int main() {
    struct sockaddr_un addr;
    int server_fd, epoll_fd;

    // Создаем серверный сокет
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCKET_PATH);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);

    epoll_fd = epoll_create1(0);

    struct epoll_event event, events[MAX_EVENTS];
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    printf("Server is listening on %s\n", SOCKET_PATH);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                // Принимаем новое подключение
                int client_fd = accept(server_fd, NULL, NULL);
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            } else {
                // Читаем один символ от клиента
                char c;
                ssize_t bytes_read = read(events[i].data.fd, &c, 1);
                if (bytes_read > 0) {
                    printf("%c", c); // Выводим символ
                    fflush(stdout);
                } else if (bytes_read == 0) {
                    // Клиент отключился
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
