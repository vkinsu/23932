#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_domain_socket"

int main() {
  int client_fd;
  struct sockaddr_un address;
  char buffer[1024];

  // Создаем сокет
  client_fd = socket(AF_UNIX, SOCK_STREAM, 0); // потоковый сокет
  if (client_fd == -1) {
    perror("\nSocket making error\n");
    exit(1);
  }

  // Настраиваем адрес сокета
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  // Устанавливаем соединение с сервером
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("\nConnection error\n");
    close(client_fd);
    exit(1);
  }

  while (fgets(buffer, sizeof(buffer), stdin)) {

  // Отправляем текст серверу
  if (write(client_fd, buffer, strlen(buffer)) == -1) {
    perror("\nWriting error");
    close(client_fd);
    exit(1);
  }
  }

  close(client_fd);

  return 0;
}