#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <pthread.h>

#define SOCKET_PATH "./unix_domain_socket"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t read_message;

    while ((read_message = read(client_socket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[read_message] = '\0';
        to_uppercase(buffer);
        printf("%s", buffer);
    }

    if (read_message == -1) {
        perror("read error");
    } else if (read_message == 0) {
        printf("client disconnected\n");
    }

    close(client_socket);
    free(arg);
    return NULL;
}

int main() {
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket error");
        return EXIT_FAILURE;
    }

    unlink(SOCKET_PATH);
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen error");
        return EXIT_FAILURE;
    }

    while (1) {
        struct sockaddr_un client_address;
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket == -1) {
            perror("accept error");
            continue;
        }

        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, client_socket_ptr) != 0) {
            perror("pthread create error");
            close(client_socket);
            free(client_socket_ptr);
        } else {
            pthread_detach(client_thread);
        }
    }

    close(server_socket);
    unlink(SOCKET_PATH);
    return EXIT_SUCCESS;
}

