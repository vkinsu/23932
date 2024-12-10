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
  int fd,cl;

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

  printf("Accepting connection...\n");
  cl = accept(fd, NULL, NULL);
  if (cl == -1)
  {
    perror("Accept error");
    exit(13);
  }


  printf("Server started.\n");

  while (1)
  {
    memset(buff, 0, BUFFER_SIZE);
    ssize_t bytes_read = read(cl, buff, BUFFER_SIZE - 1);
    if (bytes_read > 0)
    {
      if (buff[0] == '!')
      {
        printf("Exit char recived.\n");
        break;
      }
      printf("Recived:");
      for (int i = 0; buff[i] != '\0'; i++)
        printf("%c", toupper(buff[i]));
      printf("\n");
    } else {
      printf("Client disconnected.\n");
      break;
    }
  }

  printf("Server stopped.\n");
  return 0;
}
