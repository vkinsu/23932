#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

void buildLineTable(int fd, LineInfo lines[], int *lineCount) {
    off_t offset = 0;
    char buffer[MAX_LINE_LENGTH];
    ssize_t bytesRead;
    int lineIndex = 0;

    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        char *ptr = buffer;
        char *lineStart = buffer;
        while ((ptr = strchr(ptr, '\n')) != NULL) {
            lines[lineIndex].offset = offset;
            lines[lineIndex].length = ptr - lineStart + 1;
            lineIndex++;
            ptr++;
            lineStart = ptr;
            offset += lines[lineIndex - 1].length;
        }
        lseek(fd, offset, SEEK_SET);
    }

    *lineCount = lineIndex;
}

void printLine(int fd, const LineInfo *line) {
    char buffer[MAX_LINE_LENGTH];
    lseek(fd, line->offset, SEEK_SET);
    read(fd, buffer, line->length);
    buffer[line->length - 1] = '\0'; 
    printf("%s\n", buffer);
}

void printOffsetTable(LineInfo lines[], int lineCount) {
    printf("Line Number | Offset\n");
    printf("----------------------\n");
    for (int i = 0; i < lineCount; i++) {
        printf("      %3d     | %10lu\n", i + 1, (unsigned long)lines[i].offset);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    LineInfo lines[MAX_LINES];
    int lineCount = 0;
    buildLineTable(fd, lines, &lineCount);

    int lineNumber;
    printOffsetTable(lines, lineCount);

    while (1) {
        printf("Enter line number (0 to exit): ");
        scanf("%d", &lineNumber);
        if (lineNumber == 0) {
            break;
        }
        if (lineNumber > 0 && lineNumber <= lineCount) {
            printLine(fd, &lines[lineNumber - 1]);
        } else {
            printf("Invalid line number\n");
        }
    }

    close(fd);
    return 0;
}
