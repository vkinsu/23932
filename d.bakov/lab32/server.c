#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <pthread.h>

#define SOCKET_PATH "./unix_domain_socket"

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;
  char buffer[1024];
  ssize_t bytes_received;

  while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytes_received] = '\0';

    for (int j = 0; buffer[j] != '\0'; j++) {
      buffer[j] = toupper(buffer[j]);
    }

    printf("Received text: %s\n", buffer);
  }

  close(client_fd);
  free(arg);
  return NULL;
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_un address;
  struct sockaddr_un client_address;
  socklen_t client_len;
  pthread_t client_thread;

  // Создаем сокет
  server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // потоковый сокет
  if (server_fd == -1) {
    perror("\nSocket making error\n");
    exit(1);
  }

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

  while (1) {
    client_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd == -1) {
      perror("\nAccept error\n");
      continue;
    }

    int *client_fd_ptr = malloc(sizeof(int));
    *client_fd_ptr = client_fd;

    if (pthread_create(&client_thread, NULL, handle_client, client_fd_ptr) != 0) {
      perror("\nThread creation error\n");
      close(client_fd);
      free(client_fd_ptr);
    } else {
      pthread_detach(client_thread);
    }
  }

  close(server_fd);
  unlink(SOCKET_PATH);

  return 0;
}