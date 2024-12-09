#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


int sigint_count = 0;

void handle_sigint(int sig) {
    sigint_count++;
    write(STDOUT_FILENO, "\a", 1);
}

void handle_sigquit(int sig) {
    printf("\nSIGINT сигналов получено: %d\n", sigint_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    printf("Ctrl+C для звукового сигнала или Ctrl+\\ для выхода.\n");
    while (1) {
        pause();
    }
    return 0;
}