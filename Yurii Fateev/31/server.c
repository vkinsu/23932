#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <time.h>

#define SOCKET_PATH "/tmp/task31_socket"
#define BUFFER_SIZE 1024

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

void *handle_client(void *arg) {
    client_info *info = (client_info *)arg;
    int client_fd = info->client_fd;
    int client_id = info->client_id;
    char buffer[BUFFER_SIZE];

    printf("Client %d connected.\n", client_id);

    while (1) {
        int bytes = read(client_fd, buffer, sizeof(buffer));
        if (bytes > 0) {
            for (int i = 0; i < bytes; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            printf("Client %d sent: ", client_id);
            for (int i = 0; i < bytes; i++) {
                putchar(buffer[i]);
                fflush(stdout);
                delay(100);
            }
            printf("\n");
        } else {
            printf("Client %d disconnected.\n", client_id);
            close(client_fd);
            free(info);
            pthread_exit(NULL);
        }
    }
}

int main() {
    int server_fd;
    struct sockaddr_un address;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    unlink(SOCKET_PATH);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    listen(server_fd, 5);

    int client_count = 0;

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);

        client_info *info = malloc(sizeof(client_info));
        info->client_fd = client_fd;
        info->client_id = client_count++;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, info);
        pthread_detach(thread);
    }
}
