#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
	int pipe_fd[2];
	pid_t pid;
	char message[] = "Help! I'm tired of being a text in her program.";
	char buffer[BUFFER_SIZE];

	if (pipe(pipe_fd) == -1) {
		perror("Pipe failed");
		exit(1);
	}

	pid = fork();
	if (pid == -1) {
		perror("Fork failed");
		exit(1);
	}

	if (pid > 0) {
		close(pipe_fd[0]);
		write(pipe_fd[1], message, strlen(message) + 1);
		close(pipe_fd[1]);
		wait(NULL);
	} else {
		close(pipe_fd[1]);
		read(pipe_fd[0], buffer, BUFFER_SIZE);

		printf("Original message: %s\n", buffer);

		for (int i = 0; buffer[i] != '\0'; i++) {
			buffer[i] = toupper((unsigned char) buffer[i]);
		}

		printf("Converted to uppercase: %s\n", buffer);
		close(pipe_fd[0]);
		exit(0);
	}

	return 0;
}

