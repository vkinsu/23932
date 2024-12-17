#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_domain_socket"
#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <file> <delay_in_microseconds>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    int delay = atoi(argv[2]);

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection failed");
        fclose(file);
        close(client_socket);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file))
    {
        write(client_socket, buffer, strlen(buffer));
        usleep(delay);
    }

    fclose(file);
    close(client_socket);
    return EXIT_SUCCESS;
}
