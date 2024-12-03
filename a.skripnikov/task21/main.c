#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int sigint_count = 0;

void handle_sigint()
{
    printf("\a");
    fflush(NULL);
    sigint_count++;
}

void handle_sigquit()
{
    printf("\nSIGQUIT signal received. Count signals: %d\n", sigint_count);
    exit(0);
}

int main()
{
    signal(SIGINT, &handle_sigint);
    signal(SIGQUIT, &handle_sigquit);
    printf("Press Ctrl+C to hear a beep, or Ctrl+\\ to quit.\n");
    while (1)
    {
        pause();
    }
}