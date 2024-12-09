#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

void printUIDs() {
    printf("euid: %d, uid: %d\n", geteuid(), getuid());
}

void tryToOpen(char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed to open.\n");
        return;
    }

    fclose(file);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("File name not found.\n");
        return 1;
    }

    printUIDs();
    tryToOpen(argv[1]);

    setuid(getuid());

    printUIDs();
    tryToOpen(argv[1]);

    return 0;
}