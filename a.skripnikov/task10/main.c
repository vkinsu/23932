#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0)
    {
        perror("Failed to create subprocess.\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        execvp(argv[1], &argv[1]);
    }
    else
    {
        if (waitpid(pid, &status, 0) == -1)
        {
            fprintf(stderr, "WAITPID is failed. \n");
            exit(1);
        }

        if (WIFEXITED(status))
        {
            printf("Child process exited with code: %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child process was terminated by signal: %d\n", WTERMSIG(status));
        }
    }

    return 0;
}
