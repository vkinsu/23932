#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 256

int main(size_t argc, char** argv)
{
    char promt_buffer[256];

    if (argc == 1)
    {
        printf("Not enough args!\n");
        return -1;
    }
    printf("%s\n", argv[1]);

    pid_t proc = fork();

    switch (proc)
    {
    case 0:
        {
            printf("Starting user's promt from child process...\n");
            execvp(argv[1], &argv[1]);
            perror("Something went wrong in the child process!\n");
            exit(-1);
        }
    case -1: 
        {
            printf("Something went wrong!\n");
            exit(-1);
        }
    default:
        {
            printf("Parent process is waiting the child!\n");
            int stat;
            waitpid(proc, &stat, 0);

            if (WIFEXITED(stat))
            {
                int exit_code = WEXITSTATUS(stat);
                printf("The child returned %d\n", exit_code);
            }

            break;
        }
    }
    return 0;
    
}
