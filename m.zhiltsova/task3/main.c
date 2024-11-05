#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main() {
    uid_t r_id = getuid();
    printf("Real User ID: %d\n", getuid());
    printf("Effective User ID: %d\n", geteuid());

    FILE *file = fopen("mydatafile", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    fclose(file);

    if (setuid(r_id) == -1) {
        perror("Error setting user id.");
        exit(EXIT_FAILURE);
    }
    	
    printf("ID have been brought to real UID: %d\n", getuid());
    	
    file = fopen("myfile.txt", "r");
    if (file == NULL) {
        perror("Error reopening file.");
    } else {
        printf("File opened successfully after UID change.\n");
        fclose(file);
    }

    return EXIT_SUCCESS;
}

