#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFFER_SIZE 200
#define S_PATH "socket1264"


int main(int argc, char *argv[])
  {
  struct sockaddr_un s_add;
  char buff[BUFFER_SIZE];
  int fd,cl,max_fd;
  fd_set active_fds, read_fds;

  printf("Creating socket...\n");
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
  {
    perror("Cannot create socket");
    exit(13);
  }

  memset(&s_add, 0, sizeof(s_add));
  s_add.sun_family = AF_UNIX;
  strncpy(s_add.sun_path, S_PATH, sizeof(s_add.sun_path)-1);
  unlink(S_PATH);

  printf("Binding...\n");
  if (bind(fd, (struct sockaddr*)&s_add, sizeof(s_add)) == -1)
  {
    perror("Cannot bind socket");
    exit(13);
  }

  printf("Starts listening...\n");
  if (listen(fd, 5) == -1)
  {
    perror("Cannot listen socket");
    exit(13);
  }

    FD_ZERO(&active_fds);
    FD_SET(fd, &active_fds);

    max_fd = fd;

    while (1)
    {
        read_fds = active_fds;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("Select failed");
            exit(13);
        }

        for (int c_fd = 0; c_fd <= max_fd; c_fd++)
        {
            if (FD_ISSET(c_fd, &read_fds))
            {
                if (c_fd == fd)
                {

                    if ((cl = accept(fd, NULL, NULL)) == -1)
                    {
                        perror("Accept failed");
                    }
                    else
                    {
                        FD_SET(cl, &active_fds);
                        if (cl > max_fd)
                        {
                            max_fd = cl;
                        }
                        printf("New connection (fd %d)\n", cl);
                    }
                }
                else
                {

                    memset(buff, 0, BUFFER_SIZE);
                    ssize_t bytes_read = read(c_fd, buff, BUFFER_SIZE - 1);
                    if (bytes_read > 0)
                    {
                        printf("Recived (from %d): ", c_fd);
                        for (int i = 0; buff[i] != '\0'; i++)
                          printf("%c", buff[i]);
                        printf("\n");
                    }
                    else if (bytes_read == 0)
                    {

                        printf("Client %d disconnected\n", c_fd);
                        close(c_fd);
                        FD_CLR(c_fd, &active_fds);
                    }
                    else
                    {
                        perror("Read failed");
                        close(c_fd);
                        FD_CLR(c_fd, &active_fds);
                    }
                }
            }
        }
    }

  printf("Server stopped.\n");
  return 0;
}
