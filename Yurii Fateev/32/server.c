#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <time.h>

#define SOCKET_PATH "/tmp/task32_socket"
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

void delay(int milliseconds) {
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause) {
        now = clock();
    }
}

typedef struct {
    int client_fd;
    int client_id;
} client_info;

void handle_client(int client_fd, int client_id) {
    char buffer[BUFFER_SIZE];
    int bytes = read(client_fd, buffer, sizeof(buffer));
    if (bytes > 0) {
        for (int i = 0; i < bytes; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        printf("Client %d sent: ", client_id);
        for (int i = 0; i < bytes; i++) {
            putchar(buffer[i]);
            fflush(stdout);
            delay(10);
        }
        printf("\n");
    } else {
        printf("Client %d disconnected.\n", client_id);
        close(client_fd);
    }
}

int main() {
    int server_fd, epoll_fd, client_fd;
    struct sockaddr_un address;
    struct epoll_event ev, events[MAX_EVENTS];
    int client_ids[FD_SETSIZE];
    int client_count = 0;

    for (int i = 0; i < FD_SETSIZE; i++) {
        client_ids[i] = -1;
    }

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    unlink(SOCKET_PATH);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    epoll_fd = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == server_fd) {
                client_fd = accept(server_fd, NULL, NULL);
                client_ids[client_fd] = client_count++;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                printf("Client %d connected.\n", client_ids[client_fd]);
            } else {
                handle_client(events[n].data.fd, client_ids[events[n].data.fd]);
            }
        }
    }
}