#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/select.h>

#define SOCKET_PATH "./unix_domain_socket"

int main() {
  int server_fd, client_fd;
  struct sockaddr_un address;
  char buffer[1024];
  ssize_t bytes_received;
  fd_set read_fds, master_fds;
  int max_fd, new_fd;
  struct sockaddr_un client_address;
  socklen_t client_len;

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

  // добавляем серверный сокет
  FD_ZERO(&master_fds);
  FD_SET(server_fd, &master_fds);
  max_fd = server_fd;

  while (1) {
    read_fds = master_fds;
    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("\nSelect error\n");
      close(server_fd);
      exit(1);
    }

    for (int i = 0; i <= max_fd; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == server_fd) {
          // Новое соединение
          client_len = sizeof(client_address);
          new_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
          if (new_fd == -1) {
            perror("\nAccept error\n");
          } else {
            FD_SET(new_fd, &master_fds);
            if (new_fd > max_fd) {
              max_fd = new_fd;
            }
          }
        } else {
          // Читаем данные от клиента
          bytes_received = read(i, buffer, sizeof(buffer) - 1);
          if (bytes_received <= 0) {
            if (bytes_received == -1) {
              perror("\nReading from client error\n");
            }
            close(i);
            FD_CLR(i, &master_fds);
          } else {
            buffer[bytes_received] = '\0';

            for (int j = 0; buffer[j] != '\0'; j++) {
              buffer[j] = toupper(buffer[j]);
            }

            printf("Received text: %s\n", buffer);
          }
        }
      }
    }
  }

  close(server_fd);
  unlink(SOCKET_PATH);

  return 0;
}