#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

void to_uppercase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main()
{
    int server_fd, client_fd, max_fd;
    struct sockaddr_un server_addr;

    fd_set active_fds, read_fds;

    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);

    max_fd = server_fd;

    while (1)
    {
        read_fds = active_fds;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("Select failed");
            close(server_fd);
            unlink(SOCKET_PATH);
            exit(EXIT_FAILURE);
        }

        for (int fd = 0; fd <= max_fd; fd++)
        {
            if (FD_ISSET(fd, &read_fds))
            {
                if (fd == server_fd)
                {

                    if ((client_fd = accept(server_fd, NULL, NULL)) == -1)
                    {
                        perror("Accept failed");
                    }
                    else
                    {
                        FD_SET(client_fd, &active_fds);
                        if (client_fd > max_fd)
                        {
                            max_fd = client_fd;
                        }
                        printf("New client connected (fd %d)\n", client_fd);
                    }
                }
                else
                {

                    memset(buffer, 0, BUFFER_SIZE);
                    ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
                    if (bytes_read > 0)
                    {
                        to_uppercase(buffer);
                        printf("Client %d: %s\n", fd, buffer);
                    }
                    else if (bytes_read == 0)
                    {

                        printf("Client %d disconnected\n", fd);
                        close(fd);
                        FD_CLR(fd, &active_fds);
                    }
                    else
                    {
                        perror("Read failed");
                        close(fd);
                        FD_CLR(fd, &active_fds);
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
