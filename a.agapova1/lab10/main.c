#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <command> [arguments...]\n", argv[0]);
		return 1;
	}

	pid_t pid = fork();

	if (pid < 0) {
		perror("Error on fork");
		return 1;
	} else if (pid == 0) {
		execvp(argv[1], &argv[1]);
		perror("Error on execvp");
		exit(1);
	} else {
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("Error on waitpid");
			return 1;
		}

		if (WIFEXITED(status)) {
			printf("Child process exit code: %d\n", WEXITSTATUS(status));
		} else {
			printf("Child process terminated abnormally.\n");
		}
	}

	return 0;
}

