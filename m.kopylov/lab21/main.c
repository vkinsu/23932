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
    write(1, &bell, 1);
    sign++;
}


void quitsignal(int signo)
{
    printf("\nNubmer of signals = %lu\n", sign);
    exit(1);
}

int main()
{
    setbuf(stdout, NULL);

    struct sigaction sa_int, sa_quit;
    sa_int.sa_handler = intsignal;
    sa_quit.sa_handler = quitsignal;
    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_quit.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);
    sigaction(SIGQUIT, &sa_quit, NULL);

    set_term();
    atexit(reset_term);

    while (1);


    return 0;
}
