#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

int main()
{
    int client_socket;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    if ((client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connect failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Enter text to send (Ctrl+D to quit):\n");
    while (fgets(buffer, BUFFER_SIZE, stdin))
    {
        write(client_socket, buffer, strlen(buffer));
    }

    close(client_socket);
    printf("Disconnected from server.\n");

    return 0;
}