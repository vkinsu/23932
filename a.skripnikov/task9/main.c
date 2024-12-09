#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filepath>\n", argv[0]);
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
        execlp("cat", "cat", argv[1], NULL);
        perror("Failed to execute cat.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        waitpid(pid, &status, 0);
        printf("\nChild process completed. PID: %d\n", pid);
    }

    return 0;
}
