#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    pid_t pid;
    int state;

    if (argc < 2) 
    {
        printf( "Error.\n");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0)
    {
        printf( "Failed subprocess.\n");
        exit(1);
    }
    else if (pid == 0)
    {
        execvp(argv[1], &argv[1]);
        exit(EXIT_FAILURE);
    }    
    else {
        if (waitpid(pid, &state, 0) == -1)
        {
            fprintf(stderr, "waitpid failed");
            exit(1);
        }
        if (WIFEXITED(state))
            printf("Child exited with status %d\n", WEXITSTATUS(state));
        

    }
    return 0;
}