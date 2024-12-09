#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

main(int argc, char *argv[])
{
  if (argc < 2)
    {
        printf("No arguments provided\n");
        exit(13);
    }

    pid_t child;
    child = fork();

    printf("[%ld] Process start\n", getpid());

    if (child == 0)
    {
	printf("[%ld] Child process, executes <%s>\n", getuid(), argv[1]);
        int rcode = execvp(argv[1], argv+1);
	if (rcode == -1)
	{
	    perror("Failed to execute");
	    exit(13);
	}
	printf("[%ld] Finishing child process...\n", getuid());
    }
    else
    {
	printf("[%ld] Parent process, waits for child(%ld)\n", getuid(), child);
        int rcode = waitpid(child, 0, 0);
	printf("[%ld] Finishing parent process...\n", getuid());
    }
    return 0;
}
