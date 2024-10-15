#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef struct {
    off_t start;
    off_t length;
} LineInfo;

typedef struct {
    LineInfo *lines;
    size_t count;
    size_t capacity;
} LineTable;

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void init_line_table(LineTable *table) {
    table->lines = malloc(sizeof(LineInfo));
    if (table->lines == NULL) {
        handle_error("Failed to allocate memory for line table");
    }
    table->count = 0;
    table->capacity = 1;
}

void resize_line_table(LineTable *table) {
    table->capacity *= 2;
    table->lines = realloc(table->lines, table->capacity * sizeof(LineInfo));
    if (table->lines == NULL) {
        handle_error("Failed to resize line table");
    }
}

void add_line_to_table(LineTable *table, off_t start, off_t length) {
    if (table->count == table->capacity) {
        resize_line_table(table);
    }
    table->lines[table->count].start = start;
    table->lines[table->count].length = length;
    table->count++;
}

void free_line_table(LineTable *table) {
    free(table->lines);
    table->lines = NULL;
    table->count = 0;
    table->capacity = 0;
}

void print_line(const char *mapped_file, LineInfo line) {
    for (off_t i = 0; i < line.length; i++) {
        printf("%c", mapped_file[line.start + i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int file_fd = open(argv[1], O_RDONLY);
    if (file_fd == -1) {
        handle_error("Failed to open file");
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) == -1) {
        handle_error("Failed to get file statistics");
    }
    size_t file_size = file_stat.st_size;

    const char *mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    if (mapped_file == MAP_FAILED) {
        handle_error("Failed to map file into memory");
    }

    LineTable line_table;
    init_line_table(&line_table);

    off_t current_offset = 0;
    off_t line_start = 0;
    off_t line_length = 0;

    for (size_t i = 0; i < file_size; i++) {
        char c = mapped_file[i];
        if (c == '\n') {
            add_line_to_table(&line_table, line_start, line_length);
            line_start = current_offset + 1;
            line_length = 0;
        } else {
            line_length++;
        }
        current_offset++;
    }

    if (line_length > 0) {
        add_line_to_table(&line_table, line_start, line_length);
    }

    fd_set input_set;
    struct timeval timeout;

    while (1) {
        printf("Enter line number (0 to exit, blank to print all lines): ");
        fflush(stdout);

        FD_ZERO(&input_set);
        FD_SET(STDIN_FILENO, &input_set);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (!select(STDIN_FILENO + 1, &input_set, NULL, NULL, &timeout)) {
            printf("\n\n");
            for (size_t i = 0; i < line_table.count; i++) {
                print_line(mapped_file, line_table.lines[i]);
            }
            return EXIT_SUCCESS;
        }

        int line_number;
        if (scanf("%d", &line_number) != 1) {
            handle_error("Failed to read line number");
        }

        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number > line_table.count) {
            printf("Invalid line number\n");
            continue;
        }

        print_line(mapped_file, line_table.lines[line_number - 1]);
    }

    munmap((void *) mapped_file, file_size);
    free_line_table(&line_table);

    return EXIT_SUCCESS;
}
