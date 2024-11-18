#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

struct termios savtty;
int stdin_d;

void interraptCnt(int sig) {
    static int cnt = 0;
    if(sig == SIGINT){
        putchar('\a');
        cnt++;
    }
    else if(sig == SIGQUIT) {
        printf("SIGINT was received %d times.\n", cnt);
        if(tcsetattr(stdin_d, TCSAFLUSH, &savtty) == -1) {
            perror("tcsetattr");
            exit(-1);
        }
        exit(0);
    }
    if(sigset(SIGINT, interraptCnt) == SIG_ERR) {
        perror("sigset");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {

    if((stdin_d = fileno(stdin)) == -1) {
        perror("fileno");
        exit(-1);
    }
    setbuf(stdout, NULL);

    if(sigset(SIGINT, interraptCnt) == SIG_ERR) {
        perror("sigset");
        exit(-1);
    }
    if(sigset(SIGQUIT, interraptCnt) == SIG_ERR) {
        perror("sigset");
        exit(-1);
    }

    struct termios tty;
    if(tcgetattr(stdin_d, &tty) == -1){
        perror("tcgetattr");
        exit(-1);
    }
    savtty = tty;
    tty.c_lflag &= ~ECHO;
    if(tcsetattr(stdin_d, TCSAFLUSH, &tty) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    while(pause());

}