#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "SOCKET"

int main() {
    struct sockaddr_un addr;
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    connect(client_fd, (struct sockaddr *)&addr, sizeof(addr));

    const char *message = "HELLO";
    while (1) {
        for (int i = 0; i < strlen(message); i++) {
            write(client_fd, &message[i], 1);
	    usleep(5000); // Отправляем по одному символу
             // Небольшая задержка для наглядности
        }
    }

    close(client_fd);
    return 0;
}
