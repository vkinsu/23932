#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	const char *filename = "file.txt";
	pid_t pid = fork();

	if (pid < 0) {
		perror("Error when calling fork()");
		return 1;

	} else if (pid == 0) {
		execlp ("cat", "cat", filename, (char *)NULL);
		perror ("Error when calling execlp()");
		exit(1);
	} else {
		printf ("Child procces (pid: %d) created.\n", pid);
		waitpid (pid, NULL, 0);
		printf ("Child procces (pid: %d) terminated.\n", pid);
	}

	return 0;
}
