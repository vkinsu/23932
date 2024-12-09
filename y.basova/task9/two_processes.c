#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", argv);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execlp("cat", "cat", argv[1], (char *)NULL);
        perror("execlp");
        return 1;
    }


    printf("The parent process is waiting for the subprocess to finish...\n");

    wait(NULL);

    printf("The subprocess ended. The parent process continues to run.\n");

    return 0;
}
