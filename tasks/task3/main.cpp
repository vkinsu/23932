#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("Real User ID: %d\n", getuid());
    printf("Effective User ID: %d\n", geteuid());

    FILE *file = fopen("mydatafile", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fclose(file);
    return EXIT_SUCCESS;
}

