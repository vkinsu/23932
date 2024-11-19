#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() 
{
    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) 
    {
        execlp("cat", "cat", "file.txt", (char *)NULL);
        perror("execlp failed");
        exit(1);
    } 
    else
    {
        printf("Parent printing while child is running\n");

        waitpid(pid, NULL, 0);

        printf("Parent printing after child has finished\n");
    }
    return 0;
}
