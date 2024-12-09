#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define LINES_AMOUNT 1000
#define OFFSET 0L

int fd_global;
long* lines_placement_global;
int* lines_len_global;

void handle_alarm(int sig) {
    printf("\nTime's up! Printing all lines:\n");

    lseek(fd_global, 0, SEEK_SET); // Сбрасываем указатель файла на начало

    char buf[1024];
    int bytes_read;
    while ((bytes_read = read(fd_global, buf, sizeof(buf))) > 0) {
        write(1, buf, bytes_read); // Печатаем содержимое файла
    }
    printf("\n");
    free(lines_placement_global);
    free(lines_len_global);
    exit(0);
}

void fill_lines_placement(int fd, long* lines_placement, int* lines_len) {
    int curr_str_len = 0;
    int line_placement_id = 1;
    char ch;
    while (read(fd, &ch, 1)) {
        curr_str_len++;

        if (ch == '\n') {
            lines_len[line_placement_id++] = curr_str_len;
            curr_str_len = 0;
            lines_placement[line_placement_id] = lseek(fd, OFFSET, SEEK_CUR); 
        }
    }
}

void print_lines(int fd, long* lines_placement, int* lines_len) {
    int line_number;
    char bf[255];

    while (1) {
        alarm(5); // Устанавливаем таймер на 5 секунд

        printf("\nline number(0 to exit): ");
        if (scanf("%d", &line_number) == '\0') {
            break;
        }

        alarm(0); // Отключаем таймер после успешного ввода

        if (line_number == 0) {
            break;
        }

        if (line_number < 0 || line_number > LINES_AMOUNT - 1) {
            perror("line number is not valid.\n");
        } else {
            lseek(fd, lines_placement[line_number], SEEK_SET);
            if (read(fd, bf, lines_len[line_number])) {
                write(1, bf, lines_len[line_number]);
            }
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

    fd_global = fd;

    long* lines_placement = (long*)malloc(sizeof(long) * LINES_AMOUNT);
    int* lines_len = (int*)malloc(sizeof(int) * LINES_AMOUNT);

    lines_placement_global = lines_placement;
    lines_len_global = lines_len;

    lines_placement[1] = 0L; // потому что файл начинается с нулевого смещения
    fill_lines_placement(fd, lines_placement, lines_len);

    print_lines(fd, lines_placement, lines_len);

    free(lines_placement);
    free(lines_len);
    close(fd);
    return 0;
}