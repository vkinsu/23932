#include  <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <sys/un.h>


#include <sys/socket.h>

#define SOCKET_PATH "socket_path"
#define BUFFER_SIZE 1024


int main()
{
    int sock_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Error in creating");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("Error in connection");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("\n| Ctrl+D to exit |\nEnter message :\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
    {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        if (write(sock_fd, buffer, strlen(buffer)) == -1)
        {
            perror("Error in writing");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(sock_fd);
    return 0;
}
