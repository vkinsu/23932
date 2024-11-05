#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int sigint_count = 0;

void handle_sigint() {
	printf("\a");
	fflush(NULL);
	sigint_count++;
}

void handle_sigquit() {
	printf("\nSIGQUIT signal received. SIGINT signals: %d\n", sigint_count);
	exit(0);
}

int main() {
	signal(SIGINT, &handle_sigint);
	signal(SIGQUIT, &handle_sigquit);
	while (1);
}
