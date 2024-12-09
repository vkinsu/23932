#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t proc = fork();
    int stat;

    switch (proc)
    {
    case 0:
        {
            printf("Cat from child %d\n", proc);
            system("cat file.txt");
            printf("Child process ends its work\n");
            break;
        }
    case -1: 
        {
            printf("Something went wrong!\n");
            return -1;
        }
    default:
        {
            printf("Working from parent process!\nWait %d proc\n", proc);
            pid_t cpid = waitpid(proc, &stat, 0);
            printf("Open file.txt");
            freopen("file.txt", "r", stdin);
            char c;
            while ((c = getchar())!= EOF)
            {
               printf("%c", c);
            }
            break;
        }
    }
    return 0;
}
