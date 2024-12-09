#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;

    char* string_sample = "ranDoM CaSE teXT";

    if (pipe(pipefd) == -1) {
        perror("Cannot create pipe");
        exit(0);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("Cannot create fork");
        exit(0);
    }

    if (cpid == 0) {
        close(pipefd[1]);

        printf("Read from pipe: <");
        while (read(pipefd[0], &buf, 1) > 0)
            printf("%c", toupper(buf));
	printf(">\n");

        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);
        exit(0);

    } else {
        close(pipefd[0]);
	printf("Putting into pipe: <%s>\n", string_sample);
        write(pipefd[1], string_sample, strlen(string_sample));
        close(pipefd[1]);
        wait(NULL);
        exit(0);
    }
}
