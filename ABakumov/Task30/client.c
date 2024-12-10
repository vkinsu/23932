#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define S_PATH "socket1264"
#define BUFFER_SIZE 220

int main(int argc, char *argv[]) {
  struct sockaddr_un s_add;
  char buff[BUFFER_SIZE];
  int fd;

  printf("Creating socket...\n");
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Cannot create socket");
    exit(13);
  }

  memset(&s_add, 0, sizeof(s_add));
  s_add.sun_family = AF_UNIX;
  strncpy(s_add.sun_path, S_PATH, sizeof(s_add.sun_path)-1);


  printf("Connecting to server...\n");
  if (connect(fd, (struct sockaddr*)&s_add, sizeof(s_add)) == -1) {
    perror("Cannot connect to server");
    exit(13);
  }

  printf("Connected.\nPrint <!> to stop.\nInput data:\n");
  while (fgets(buff, BUFFER_SIZE, stdin) != NULL)
  {
    if (write(fd, buff, strlen(buff)) == -1)
    {
      perror("Cannot write into socket");
      exit(13);
    }
    if (buff[0] == '!')
    {
      printf("Exit...\n");
      break;
    }
  }

  printf("Disconnected from server.\n");
  return 0;
}
