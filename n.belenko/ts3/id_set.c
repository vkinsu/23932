#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main() {
    FILE *fp;
    fp = fopen("secret_data.txt", "w+");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    fclose(fp);
    if (chmod("secret_data.txt", S_IRUSR | S_IWUSR) == -1) {
        perror("chmod");
        exit(1);
    }

    printf("Real user ID: %d\n", getuid());
    printf("User EID: %d\n", geteuid());

    fp = fopen("secret_data.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    fclose(fp);

    if (setuid(getuid()) == -1) {
        perror("setuid");
        exit(1);
    }

    printf("\nAfter setuid():\n");
    printf("Real user ID: %d\n", getuid());
    printf("User EID: %d\n", geteuid());

    return 0;
}


