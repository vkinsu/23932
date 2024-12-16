#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./unix_domain_socket"

int main() {
  int server_fd, client_fd;
  struct sockaddr_un address;
  char buffer[1024];
  ssize_t bytes_received;

  // Создаем сокет
  server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // потоковый сокет
  if (server_fd == -1) {
    perror("\nSocket making error\n");
    exit(1);
  }

  // Удаляем существующий сокет файл, если он есть
  unlink(SOCKET_PATH);

  // Настраиваем адрес сокета
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  // Привязываем сокет к адресу
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("\nServer connection error\n");
    close(server_fd);
    exit(1);
  }

  // Слушаем на сокете
  if (listen(server_fd, 5) == -1) {
    perror("\nSocket listening error\n");
    close(server_fd);
    exit(1);
  }

  // Принимаем соединение
  client_fd = accept(server_fd, NULL, NULL);
  if (client_fd == -1) {
    perror("\nAccept error\n");
    close(server_fd);
    exit(1);
  }

  while(1) {
    // Читаем данные от клиента
    bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received == -1) {
      perror("\nReading from client error\n");
      close(client_fd);
      close(server_fd);
      exit(1);
    }

    if (buffer[0] == '\0') {
      break;
    } else {
      buffer[bytes_received] = '\0';

      for (int i = 0; buffer[i] != '\0'; i++) {
        buffer[i] = toupper(buffer[i]);
      }
      
      printf("Received text: %s\n", buffer);
    }

  }
  close(client_fd);
  close(server_fd);

  unlink(SOCKET_PATH);

  return 0;
}