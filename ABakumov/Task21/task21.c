#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int counter = 0;

void handler_sigint(int sig) {
    counter++;
    printf("\aBeep\n");
    signal(SIGINT, handler_sigint);
    fflush(stdout);
}

void handler_sigquit(int sig) {
    printf("\nTotal: %d\n", counter);
    exit(0);
}

int main() {
    signal(SIGINT, handler_sigint);
    signal(SIGTSTP, handler_sigquit);
    printf("Ctrl+C -> signal, Ctrl+Z -> exit\n");
    while (1) { pause();}
    return 0;
}
