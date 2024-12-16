#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/task31_socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un address;

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    printf("Connecting to the server\n");
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    connect(client_fd, (struct sockaddr *)&address, sizeof(address));

    printf("Enter the text to send to the server (Ctrl+D to exit):\n");
    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        write(client_fd, buffer, bytes);
    }

    close(client_fd);
    printf("The connection to the server is closed.\n");
    return 0;
}
