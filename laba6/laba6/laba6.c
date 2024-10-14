#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>

typedef struct {
    off_t offset;
    off_t length;
} Line;

typedef struct {
    Line* array;
    int cnt;
    int cap;
} Array;

void initArray(Array* a) {
    a->array = malloc(sizeof(Line));
    a->cnt = 0;
    a->cap = 1;
}

void insertArray(Array* a, Line element) {
    if (a->cnt == a->cap) {
        a->cap *= 2;
        a->array = realloc(a->array, a->cap * sizeof(Line));
    }

    a->array[a->cnt++] = element;
}

void freeArray(Array* a) {
    free(a->array);
    a->array = NULL;
    a->cnt = a->cap = 0;
}

void printLine(Line line, int fd) {
    char* buf = calloc(line.length + 1, sizeof(char));

    lseek(fd, line.offset, SEEK_SET);
    read(fd, buf, line.length * sizeof(char));

    printf("%s\n", buf);
    free(buf);
}

int main(int argc, char* argv[]) {
    if (argc != 2) { return 1; }
    char* path = argv[1];

    Array table;
    initArray(&table);

    int fd = open(path, O_RDONLY);
    if (fd == -1) { return 1; }
    lseek(fd, 0L, SEEK_CUR);

    char c;
    off_t lineOffset = 0;
    off_t lineLength = 0;
    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            Line current = { lineOffset, lineLength };
            insertArray(&table, current);

            lineOffset += lineLength + 1;
            lineLength = 0;
        }
        else {
            lineLength++;
        }
    }

    if (lineLength > 0) {
        Line current = { lineOffset, lineLength };
        insertArray(&table, current);
    }

    fd_set fdset;
    struct timeval timeout;

    while (1) {
        printf("Enter the line number: ");
        fflush(stdout);

        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (!select(1, &fdset, NULL, NULL, &timeout)) {
            printf("\n\n");
            for (int i = 0; i < table.cnt; i++)
                printLine(table.array[i], fd);
            return 0;
        }

        int num;
        scanf("%d", &num);

        if (num == 0) { break; }
        if (table.cnt < num) {
            printf("The file contains only %d line(s).\n", table.cnt);
            continue;
        }

        Line line = table.array[num - 1];
        printLine(line, fd);
    }

    close(fd);
    freeArray(&table);

    return 0;
}