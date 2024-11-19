#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;

void sigint_handler(int signum) {
    sigint_count++;
    printf("\a"); 
    fflush(stdout);
}

void sigquit_handler(int signum) {
    printf("Received %d SIGINT signals\n", sigint_count);
    exit(0);
}

int main() {
    struct sigaction sa_int, sa_quit;

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = sigquit_handler;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Press CTRL-C to emit a beep sound.\n");
    printf("Press CTRL-\\ to exit the program.\n");

    while (1) {
        pause();
    }

    return 0;
}
