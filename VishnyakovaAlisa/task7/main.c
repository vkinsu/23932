#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

void handle_alarm(int var) {
    puts("\nTime's up.");
    _Exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    char* mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    off_t offsets[MAX_LINES];
    int line_lengths[MAX_LINES];
    int line_count = 0;

    char* ptr = mapped;
    char* end_ptr = mapped + sb.st_size;
    while (ptr < end_ptr && line_count < MAX_LINES) {
        offsets[line_count] = ptr - mapped;
        char* newline = memchr(ptr, '\n', end_ptr - ptr);
        if (newline) {
            line_lengths[line_count] = newline - ptr + 1;
            ptr = newline + 1;
        }
        else {
            line_lengths[line_count] = end_ptr - ptr;
            ptr = end_ptr;
        }
        line_count++;
    }

    printf("Line offsets and lengths:\n");
    for (int i = 0; i < line_count; i++) {
        printf("Line %d: Offset = %ld, Length = %d\n", i + 1, offsets[i], line_lengths[i]);
    }

    while (1) {
        int line_number;
        printf("Enter line number (0 to quit): ");
        signal(SIGALRM, handle_alarm);
        alarm(5);
        scanf("%d", &line_number);
        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number > line_count) {
            fprintf(stderr, "Invalid line number. Please try again.\n");
            continue;
        }

        printf("Line %d: ", line_number);
        fwrite(mapped + offsets[line_number - 1], 1, line_lengths[line_number - 1], stdout);
        printf("\n");
    }

    munmap(mapped, sb.st_size);
    return 0;
}
