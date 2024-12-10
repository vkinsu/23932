#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_domain_socket"
#define BUFFER_SIZE 256

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <delay>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    int delay = atoi(argv[2]);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen error");
        return EXIT_FAILURE;
    }

    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket error");
        fclose(fp);
        return EXIT_FAILURE;
    }

    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path));

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect error");
        close(client_socket);
        fclose(fp);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        if (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
            if (write(client_socket, buffer, strlen(buffer)) == -1) {
                perror("write error");
                close(client_socket);
                fclose(fp);
                return EXIT_FAILURE;
            }
        }

        if (feof(fp)) {
            rewind(fp);
        }

        usleep(delay);
    }

    fclose(fp);
    close(client_socket);
    return EXIT_SUCCESS;
}

