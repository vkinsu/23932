#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int main() 
{
    FILE* file;
    const char* filename = "file";

    printf("Before changes:\n");

    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error");
    }

    else {
        printf("File opened.\n");
        fclose(file);
    }

    if (setuid(getuid()) == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    printf("After changes\n");

    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error");
    }

    else {
        printf("File opened\n");
        fclose(file);
    }
    return 0;
}