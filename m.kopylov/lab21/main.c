#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

struct termios local_term;

void set_term()
{
    struct termios attr;
    tcgetattr(STDIN_FILENO, &local_term);
    memcpy(&attr, &local_term, sizeof(attr));

    attr.c_iflag = ~(ICANON|ECHO);
    attr.c_iflag = ISIG;
    attr.c_cc[VINTR] = 'c';
    attr.c_cc[VQUIT] = 'q';

    tcsetattr(STDERR_FILENO, TCSAFLUSH, &attr);
    return;
}

void reset_term()
{
    printf("Reseting...\n");
    tcsetattr(STDERR_FILENO, TCSANOW, &local_term);
}

static size_t sign = 0;

void intsignal(int signo)
{
    char bell = '\a';
    while (1) 
    {
	write(1, &bell, 1);
        sign++;
    }
}

void quitsignal(int signo)
{
    printf("\nNubmer of signals = %lu\n", sign);
    exit(1);
}

int main()
{
    setbuf(stdout, NULL);
    set_term();
    atexit(reset_term);
    signal(SIGINT, intsignal);
    signal(SIGQUIT, quitsignal);

    while (1) {};

    return 0;
}
