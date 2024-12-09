#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char buffer[MAX_LINE_LENGTH];
    off_t offsets[MAX_LINES];
    int line_lengths[MAX_LINES];
    int line_count = 0;

    off_t current_offset = 0;
    while (1) {
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;

        buffer[bytes_read] = '\0';
        
        char *ptr = buffer;
        while (*ptr && line_count < MAX_LINES) {
            offsets[line_count] = current_offset;
            char *newline = strchr(ptr, '\n');
            if (newline) {
                line_lengths[line_count] = newline - ptr + 1; 
                ptr = newline + 1;
            } else {
                line_lengths[line_count] = strlen(ptr);
                ptr += strlen(ptr);
            }
            current_offset += line_lengths[line_count];
            line_count++;
        }
    }

    close(fd);

    printf("Line offsets and lengths:\n");
    for (int i = 0; i < line_count; i++) {
        printf("Line %d: Offset = %ld, Length = %d\n", i+1, offsets[i], line_lengths[i]);
    }

    while (1) {
        int line_number;
        printf("Enter line number (0 to quit): ");
        scanf("%d", &line_number);
        if (line_number == 0) {
            break;
        }

        if (line_number < 0 || line_number > line_count) {
            printf("Invalid line number. Please try again.\n");
            continue;
        }

        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("open");
            return 1;
        }

        lseek(fd, offsets[line_number-1], SEEK_SET);

        char line[MAX_LINE_LENGTH];
        ssize_t bytes_to_read = line_lengths[line_number-1];
        read(fd, line, bytes_to_read);
        line[bytes_to_read] = '\0';

        printf("Line %d: %s", line_number, line);

        close(fd);
    }

    return 0;
}

