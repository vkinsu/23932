#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

#define MAX_LINES 1000

typedef struct
{
    off_t offset;
    size_t length;
} Line;

Line lines[MAX_LINES];
int count = 0;
char *mapped_file = NULL;
size_t file_size = 0;

void print_entire_file()
{
    write(STDOUT_FILENO, mapped_file, file_size);
    printf("\n");
}

void handle_alarm(int sig)
{
    printf("\nTime is up! Displaying the entire file:\n");
    print_entire_file();
    exit(0);
}

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

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0)
    {
        perror("Error getting file size");
        close(fd);
        exit(1);
    }
    file_size = file_stat.st_size;

    mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped_file == MAP_FAILED)
    {
        perror("Error mapping file");
        close(fd);
        exit(1);
    }

    close(fd);

    off_t start_of_line = 0;
    for (off_t i = 0; i < file_size; i++)
    {
        if (mapped_file[i] == '\n')
        {
            lines[count].offset = start_of_line;
            lines[count].length = i - start_of_line + 1;
            start_of_line = i + 1;
            count++;
        }
    }

    if (start_of_line < file_size)
    {
        lines[count].offset = start_of_line;
        lines[count].length = file_size - start_of_line;
        count++;
    }

    char input[16];
    int line_number;

    signal(SIGALRM, handle_alarm);

    while (1)
    {
        printf("\nEnter the line number to display (0 to exit): ");
        alarm(5);

        if (!fgets(input, sizeof(input), stdin))
        {
            printf("Error reading input.\n");
            continue;
        }

        alarm(0);

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

        Line line = lines[line_number - 1];
        write(STDOUT_FILENO, mapped_file + line.offset, line.length);
    }

    munmap(mapped_file, file_size);
    return 0;
}
