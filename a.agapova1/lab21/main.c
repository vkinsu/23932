#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int sig) {
	sigint_count++;
	printf("\a");
}

void handle_sigquit(int sig) {
	printf("\nSIGQUIT signal received. SIGINT signals: %d\n", sigint_count);
	exit(0);
}

int main() {
	struct sigaction sa_int, sa_quit;

	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;

	sa_quit.sa_handler = handle_sigquit;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;

	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);

	while (1) {
		pause();
	}

	return 0;
}

