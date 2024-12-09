#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINES 1000
#define MAX_SIZE_BUFFER 256

typedef struct
{
    off_t offset;
    size_t length;
} Line;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: <filepath>\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Error opening the file: %s\n", argv[1]);
        exit(1);
    }

    Line lines[MAX_LINES];
    int count = 0;
    int bytes_read = 0;
    char buffer[1];
    off_t start_of_line = 0;

    while ((bytes_read = read(fd, buffer, 1)) > 0)
    {
        if (buffer[0] == '\n')
        {
            off_t current_offset = lseek(fd, 0L, 1);
            lines[count].offset = start_of_line;
            lines[count].length = current_offset - start_of_line;
            start_of_line = current_offset;
            count++;
        }
    }

    if (start_of_line < lseek(fd, 0L, SEEK_END))
    {
        lines[count].offset = start_of_line;
        lines[count].length = lseek(fd, 0L, SEEK_END) - start_of_line;
        count++;
    }

    printf("\nOffsets and line lengths table:\n");
    for (int i = 0; i < count; i++)
    {
        printf("Line %d: offset = %ld, length = %zu\n", i + 1, lines[i].offset, lines[i].length);
    }

    char input[16];
    int line_number;
    while (1)
    {
        printf("\nEnter the line number to display. 0 to exit. The number: ");
        if (!fgets(input, sizeof(input), stdin))
        {
            printf("Error reading input.\n");
            continue;
        }

        char *endptr;
        line_number = strtol(input, &endptr, 10);

        if (*endptr != '\n' && *endptr != '\0')
        {
            printf("Invalid input! Please enter a valid number.\n");
            continue;
        }

        if (line_number == 0)
            break;

        if (line_number < 1 || line_number > count)
        {
            printf("Incorrect line number!\n");
            continue;
        }

        char *line_buffer = malloc(lines[line_number - 1].length + 1);

        Line line = lines[line_number - 1];
        lseek(fd, line.offset, SEEK_SET);
        read(fd, line_buffer, line.length);
        line_buffer[line.length] = '\0';

        printf("Line %d: %s", line_number, line_buffer);
        free(line_buffer);
    }

    close(fd);
    return 0;
}