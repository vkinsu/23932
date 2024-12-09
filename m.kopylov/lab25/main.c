#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char** argv)
{
    int descriptors[2];
    char read_char;
    char new_line = '\n';

    if (pipe(descriptors) == -1)
    {
        perror("pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t proc = fork();

    switch (proc)
    {
    case 0:
        {
            close(descriptors[1]);
            printf("Child proc is reading string:\n\n");
            while (read(descriptors[0], &read_char, sizeof(char)) != 0)
            {
                read_char = toupper(read_char);
                write(STDOUT_FILENO, &read_char, 1);
            }
            write(STDOUT_FILENO, &new_line, 1);
            close(descriptors[1]);
            exit(EXIT_SUCCESS);
        }
    case -1: 
        {
            perror("fork\n");
            exit(EXIT_FAILURE);
        }
    default:
        {
            close(descriptors[0]);
            printf("Sending string to child proc\n");
            write(descriptors[1], argv[1], strlen(argv[1]));
            close(descriptors[1]);
            printf("Waiting for the child proc\n");
            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }
    return 0;
}
