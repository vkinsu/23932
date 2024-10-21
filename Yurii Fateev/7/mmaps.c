#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#define LINES_AMOUNT 1000
#define OFFSET 0L

long* lines_placement_global;
int* lines_len_global;
char* file_map;
size_t file_size;

void handle_alarm(int sig) {
    printf("\nTime's up! Printing all lines:\n");

    // Печатаем содержимое файла из отображенной памяти
    write(1, file_map, file_size);

    free(lines_placement_global);
    free(lines_len_global);
    munmap(file_map, file_size); // Освобождаем отображенную память
    exit(0);
}

void fill_lines_placement(char* file_content, long* lines_placement, int* lines_len, size_t size) {
    int curr_str_len = 0;
    int line_placement_id = 1;

    for (size_t i = 0; i < size; i++) {
        curr_str_len++;

        if (file_content[i] == '\n') {
            lines_len[line_placement_id++] = curr_str_len;
            curr_str_len = 0;
            lines_placement[line_placement_id] = i + 1;
        }
    }
}

void print_lines(char* file_content, long* lines_placement, int* lines_len) {
    int line_number;

    while (1) {
        alarm(5);

        printf("\nline number(0 to exit): ");
        if (scanf("%d", &line_number) == '\0') {
            break;
        }

        alarm(0);

        if (line_number == 0) {
            break;
        }

        if (line_number < 0 || line_number > LINES_AMOUNT - 1) {
            perror("line number is not valid.\n");
        } else {
            // Печатаем строку из отображенной памяти
            write(1, &file_content[lines_placement[line_number]], lines_len[line_number]);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: <filepath>\n");
        exit(1);
    }

    // Устанавливаем обработчик сигнала SIGALRM
    signal(SIGALRM, handle_alarm);

    int fd;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror(strcat("error opening the file: ", argv[1]));
        exit(1);
    }

    // Получаем размер файла
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("fstat error");
        close(fd);
        exit(1);
    }
    file_size = file_stat.st_size;

    // Отображаем файл в память
    file_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        perror("mmap error");
        close(fd);
        exit(1);
    }

    long* lines_placement = (long*)malloc(sizeof(long) * LINES_AMOUNT);
    int* lines_len = (int*)malloc(sizeof(int) * LINES_AMOUNT);

    lines_placement_global = lines_placement;
    lines_len_global = lines_len;

    lines_placement[1] = 0L; // потому что файл начинается с нулевого смещения
    fill_lines_placement(file_map, lines_placement, lines_len, file_size);

    print_lines(file_map, lines_placement, lines_len);

    // Освобождаем ресурсы
    free(lines_placement);
    free(lines_len);
    munmap(file_map, file_size);
    close(fd);
    return 0;
}