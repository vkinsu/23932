#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int file_fd = open(argv[1], O_RDONLY);
    if (file_fd == -1) {
        handle_error("Failed to open file");
    }

    LineTable line_table;
    init_line_table(&line_table);

    off_t current_offset = 0;
    off_t line_start = 0;
    off_t line_length = 0;
    char c;

    while (read(file_fd, &c, 1) == 1) {
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

    int line_number;
    while (1) {
        printf("Enter line number (0 to exit): ");
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

        LineInfo line = line_table.lines[line_number - 1];
        char *buffer = malloc(line.length + 1);
        if (buffer == NULL) {
            handle_error("Failed to allocate memory for buffer");
        }

        lseek(file_fd, line.start, SEEK_SET);
        if (read(file_fd, buffer, line.length) != line.length) {
            handle_error("Failed to read line from file");
        }

        buffer[line.length] = '\0';
        printf("%s\n", buffer);

        free(buffer);
    }

    close(file_fd);
    free_line_table(&line_table);

    return EXIT_SUCCESS;
}
