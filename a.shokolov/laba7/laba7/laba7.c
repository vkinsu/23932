#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/stat.h>

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

void printLine(Line line, const char* mapped) {
    for (int i = 0; i < line.length; i++) {
        printf("%c", mapped[line.offset + i]);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) { return 1; }
    char* path = argv[1];

    Array table;
    initArray(&table);

    int fd = open(path, O_RDONLY);
    if (fd == -1) { return 1; }

    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1) { return 1; }
    size_t size = fileInfo.st_size;

    const char* mapped = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) { return 1; }

    off_t lineOffset = 0;
    off_t lineLength = 0;

    for (int i = 0; i < size; i++) {
        char c = mapped[i];
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
        FD_SET(0, &fdset); // STDIN_FILENO = 0
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (!select(1, &fdset, NULL, NULL, &timeout)) {
            printf("\n\n");
            for (int i = 0; i < table.cnt; i++)
                printLine(table.array[i], mapped);
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
        printLine(line, mapped);
    }

    munmap((void*)mapped, size);
    freeArray(&table);

    return 0;
}