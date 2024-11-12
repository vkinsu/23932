#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256
#define TIMEOUT_SECONDS 5

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

void buildLineTable(char *fileContent, size_t fileSize, LineInfo lines[], int *lineCount) {
    char *ptr = fileContent;
    char *lineStart = fileContent;
    int lineIndex = 0;

    while (ptr < fileContent + fileSize) {
        if (*ptr == '\n') {
            lines[lineIndex].offset = lineStart - fileContent;
            lines[lineIndex].length = ptr - lineStart + 1;
            lineIndex++;
            ptr++;
            lineStart = ptr;
        } else {
            ptr++;
        }
    }

    *lineCount = lineIndex;
}

void printLine(char *fileContent, const LineInfo *line) {
    char *lineStart = fileContent + line->offset;
    char *lineEnd = lineStart + line->length;
    while (lineStart < lineEnd && *lineStart != '\n') {
        printf("%c", *lineStart);
        lineStart++;
    }
    printf("\n");
}

void printOffsetTable(LineInfo lines[], int lineCount) {
    printf("Line Number | Offset\n");
    printf("----------------------\n");
    for (int i = 0; i < lineCount; i++) {
        printf("      %3d     | %10lu\n", i + 1, (unsigned long)lines[i].offset);
    }
}

void timeout_handler(int sig) {
    printf("\nTimeout!\n");
    exit(0);
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

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    char *fileContent = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (fileContent == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    LineInfo lines[MAX_LINES];
    int lineCount = 0;
    buildLineTable(fileContent, sb.st_size, lines, &lineCount);
    printOffsetTable(lines, lineCount);

    int lineNumber;
    while (1) {
        printf("Enter line number (0 to exit): ");
        fflush(stdout);

        signal(SIGALRM, timeout_handler);
        alarm(TIMEOUT_SECONDS);

        if (scanf("%d", &lineNumber) != 1) {
            // Invalid input, assume timeout
            break;
        }

        alarm(0);

        if (lineNumber == 0) {
            break;
        }
        if (lineNumber > 0 && lineNumber <= lineCount) {
            printLine(fileContent, &lines[lineNumber - 1]);
        } else {
            printf("Invalid line number\n");
        }
    }

    if (munmap(fileContent, sb.st_size) == -1) {
        perror("munmap");
    }
    close(fd);
    return 0;
}
