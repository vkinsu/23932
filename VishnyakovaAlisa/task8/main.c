#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();
    char* filepath = "/bin/cat";
    char* argv[] = { filepath, "main.c", NULL };
    if (pid < 0) {
        perror("Failed to create subprocess.\n");
        exit(1);
    }
    if (pid == 0) {
        int t = execve(filepath, argv, NULL);
        printf("2 %d\n", t);
        exit(1);
    }
    else {
        waitpid(pid, &status, 0);
        printf("\nChild finished. Its pid was:  %d\n", pid);
    }
    return 0;
}