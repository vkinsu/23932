#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define CONST_STRING_SAMPLE "raNDoM cASe tExT"

int main() {
    char* str = CONST_STRING_SAMPLE;

    int pipe_fds[2];
    pid_t pid;

    if (pipe(pipe_fds) == -1) {
        perror("Cannot create pipe");
        exit(13);
    }

    if ((pid = fork()) == -1) {
        perror("Cannot fork process");
        exit(13);
    }

    if (pid == 0) {
        close(pipe_fds[0]);
        printf("Putting into pipe: <%s>\n", str);
        write(pipe_fds[1], str, strlen(str));
        close(pipe_fds[1]);
        exit(0);
    } else {
        close(pipe_fds[1]);
	waitpid(pid, 0, 0);
        char* buff[BUFFER_SIZE];
	read(pipe_fds[0], buff, strlen(str));
	printf("Read from pipe and converted to upper: <");
	for (int i = 0; i < strlen(buff); i++)
	    printf("%c", toupper(buff[i]));
	printf(">\n");
        close(pipe_fds[0]);
        exit(0);
    }

    return 0;
}
