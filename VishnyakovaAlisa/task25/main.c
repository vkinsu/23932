#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Создаем канал
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаем подпроцесс
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        close(pipefd[1]); // Закрываем конец записи

        // Читаем из канала и преобразуем в верхний регистр
        while (read(pipefd[0], buffer, BUFFER_SIZE) > 0) {
            for (int i = 0; buffer[i] != '0'; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            printf("Up registor: %s\n", buffer);
        }

        close(pipefd[0]); // Закрываем конец чтения
        exit(EXIT_SUCCESS);
    }
    else { // Родительский процесс
        close(pipefd[0]); // Закрываем конец чтения

        // Отправляем текст в канал
        const char* text = "Hello, our World!\nThis is a test.\n";
        write(pipefd[1], text, strlen(text) + 1); // +1 для передачи нуль-терминатора

        close(pipefd[1]); // Закрываем конец записи
        wait(NULL); // Ожидаем завершения дочернего процесса
        exit(EXIT_SUCCESS);
    }
}
