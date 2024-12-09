#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int main()
{
    int pipe_fd[2]; // pipe_fd[0] — чтение, pipe_fd[1] — запись
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        close(pipe_fd[1]);

        int nread;
        while ((nread = read(pipe_fd[0], buffer, BUFFER_SIZE)) > 0)
        {

            for (int i = 0; i < nread; i++)
            {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }

            write(STDOUT_FILENO, buffer, nread);
        }

        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    }
    else
    { // Родительский процесс
        close(pipe_fd[0]);

        char *message = "Hello, World! \n";
        write(pipe_fd[1], message, strlen(message));

        close(pipe_fd[1]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
