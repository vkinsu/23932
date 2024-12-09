#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

typedef struct line_info {
    int indentation;
    int length;
} line_info;

typedef struct vector {
    line_info* data;
    int capacity;
    int count;
} vector;

vector vector_init() {
    vector v;
    v.data = (line_info*)malloc(sizeof(line_info));
    v.capacity = 1;
    v.count = 0;
    return v;
}

void vector_add(vector* v, line_info* data) {
    if (v != NULL) {
        if (v->capacity == v->count) {
            v->capacity *= 2;
            line_info* check = (line_info*)realloc(v->data, v->capacity * sizeof(line_info));
            if (check != NULL)
                v->data = check;
            else
                return;
        }
        v->data[v->count++] = *data;
    }
}

void vector_destroy(vector* v) {
    if (v != NULL)
        free(v->data);
}

vector build_table(int descriptor) {
    vector table = vector_init();
    char symbol;
    int indentation = 0;
    int length = 0;
    while (1) {
        long f = read(descriptor, &symbol, 1);
        if (symbol == '\n' || f == 0) {
            line_info data = { indentation, length };
            vector_add(&table, &data);
            indentation += length + 1;
            length = 0;
            if (f == 0)
                break;
        }
        else
            length++;
    }
    return table;
}

void print_file(int descriptor) {
    lseek(descriptor, 0, SEEK_SET);
    char buffer[256];
    int bytesRead;
    while ((bytesRead = read(descriptor, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }
}

int main() {
    int descriptor = open("input.txt", O_RDONLY);
    if (descriptor == -1) {
        perror("Error opening file");
        exit(1);
    }

    vector table = build_table(descriptor);
    while (1) {
        printf("Enter the number of a line: ");
        fflush(stdout);

        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        if (rv == -1) {
            perror("select");
        } else if (rv == 0) {
            printf("\nTimeout! Printing file content:\n");
            print_file(descriptor);
            break;
        } else {
            char digits[32];
            fgets(digits, 32, stdin);
            long number = strtol(digits, NULL, 10);
            if (number == 0)
                break;
            if (0 < number && number <= table.count) {
                lseek(descriptor, table.data[number - 1].indentation, SEEK_SET);
                char line[table.data[number - 1].length + 1];
                read(descriptor, line, table.data[number - 1].length);
                line[table.data[number - 1].length] = '\0';
                printf("%s\n", line);
            } else {
                printf("Line number out of range\n");
            }
        }
    }

    vector_destroy(&table);
    close(descriptor);
    return 0;
}
