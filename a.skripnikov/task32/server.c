#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define SOCKET_PATH "./unix_domain_socket"
#define BUFFER_SIZE 256
#define MAX_EVENTS 20

void to_uppercase(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket error");
        return EXIT_FAILURE;
    }

    unlink(SOCKET_PATH);
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind error");
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("listen error");
        return EXIT_FAILURE;
    }

    if (set_nonblocking(server_socket) == -1)
    {
        perror("failed to set non-blocking mode");
        return EXIT_FAILURE;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll create error");
        return EXIT_FAILURE;
    }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &ev) == -1)
    {
        perror("epoll_ctl error");
        return EXIT_FAILURE;
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1)
        {
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < n; i++)
        {
            if (events[i].data.fd == server_socket)
            {
                // New client connection
                struct sockaddr_un client_address;
                socklen_t client_len = sizeof(client_address);
                int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
                if (client_socket == -1)
                {
                    perror("accept error");
                    continue;
                }

                printf("New client connected (fd %d)\n", client_socket);

                if (set_nonblocking(client_socket) == -1)
                {
                    perror("failed to set non-blocking mode for client");
                    close(client_socket);
                    continue;
                }

                ev.events = EPOLLIN | EPOLLET; // Edge-triggered mode
                ev.data.fd = client_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) == -1)
                {
                    perror("epoll_ctl add client error");
                    close(client_socket);
                }
            }
            else
            {
                // Data available on client socket
                char buffer[BUFFER_SIZE];
                int client_socket = events[i].data.fd;
                ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';
                    to_uppercase(buffer);

                    // Ensure proper formatting
                    printf("Client %d: %s\n", client_socket, buffer); // Print with a newline
                }
                else if (bytes_read == 0)
                {
                    // Client disconnected
                    printf("Client %d disconnected\n", client_socket);
                    close(client_socket);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                }
                else
                {
                    perror("read error");
                }
            }
        }
    }

    close(server_socket);
    unlink(SOCKET_PATH);
    return EXIT_SUCCESS;
}
