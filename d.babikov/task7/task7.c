#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

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

vector build_table(const char* mapped_data, size_t file_size) {
    vector table = vector_init();
    int indentation = 0;
    int length = 0;

    for (size_t i = 0; i < file_size; ++i) {
        char symbol = mapped_data[i];
        if (symbol == '\n') {
            line_info data = { indentation, length };
            vector_add(&table, &data);
            indentation += length + 1;
            length = 0;
        } else {
            length++;
        }
    }

    if (length > 0) {
        line_info data = { indentation, length };
        vector_add(&table, &data);
    }

    return table;
}

int main() {
    int descriptor = open("input.txt", O_RDONLY);
    if (descriptor == -1) {
        perror("Error opening file");
        exit(1);
    }

    struct stat sb;
    if (fstat(descriptor, &sb) == -1) {
        perror("Error getting file size");
        close(descriptor);
        exit(1);
    }

    size_t file_size = sb.st_size;
    char* mapped_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (mapped_data == MAP_FAILED) {
        perror("Error mapping file");
        close(descriptor);
        exit(1);
    }

    vector table = build_table(mapped_data, file_size);

    while (1) {
        printf("Enter the number of a line: ");
        char digits[32];
        fgets(digits, 32, stdin);
        long number = strtol(digits, NULL, 10);
        if (number == 0)
            break;
        
        if (0 < number && number <= table.count) {
            line_info line = table.data[number - 1];
            printf("%.*s\n", line.length, mapped_data + line.indentation);
        } else {
            printf("Invalid line number. Please try again.\n");
        }
    }

    vector_destroy(&table);

    close(descriptor);
    return 0;
}
