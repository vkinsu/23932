#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        //fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        fprintf(stderr, "Few arguments\n");
        printf("Few arguments\n");
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork didn't create\n");
        return 1;
    }
    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execvp error\n");
        exit(1);
    }
    else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid error\n");
            return 1;
        }
        if (WIFEXITED(status)) {
            printf("Exit code: %d\n", WEXITSTATUS(status));
        }
        else {
            printf("Process did not terminate normally.\n");
        }
    }
    return 0;
}
