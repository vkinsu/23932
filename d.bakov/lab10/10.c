#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("There are not enough arguments for this program!");
        exit(EXIT_FAILURE);
    }
    pid_t process_id = fork();
    int status;
    switch (process_id) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            execvp(argv[1], &argv[1]);
            exit(1);
        default:
            waitpid(process_id, &status, 0);
            if (WIFEXITED(status))
                printf("Exit code of the descendant process: %d\n", WEXITSTATUS(status));
    }
    return 0;
}