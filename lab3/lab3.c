#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_user_ids() {
    printf("Effective User ID: %d\n", geteuid());
    printf("Real User ID: %d\n", getuid());
}

int open_file(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    fclose(file);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    printf("Before setuid:\n");
    print_user_ids();

    if (open_file(argv[1]) != 0) {
        return 1;
    }

    if (setuid(getuid()) != 0) {
        perror("Error setting user ID");
        return 1;
    }

    printf("After setuid:\n");
    print_user_ids();

    if (open_file(argv[1]) != 0) {
        return 1;
    }

    return 0;
}
