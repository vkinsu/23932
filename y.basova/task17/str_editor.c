#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>

#define ERASE 0x7F 
#define KILL 0x15 
#define CTRL_W 0x17
#define CTRL_D 0x04
#define CTRL_G 0x07

void set_terminal_mode(int enable) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    if (enable) {
        ttystate.c_lflag &= ~(ICANON | ECHO);
        ttystate.c_cc[VMIN] = 1;
        ttystate.c_cc[VTIME] = 0;
    } else {
        ttystate.c_lflag |= (ICANON | ECHO);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void handle_input(char *buffer, int *length) {
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == ERASE) {
            if (*length > 0) {
                printf("\b \b");
                (*length)--;
            }
        } else if (c == KILL) {
            while (*length > 0) {
                printf("\b \b");
                (*length)--;
            }
        } else if (c == CTRL_W) {
            while (*length > 0 && isspace(buffer[*length - 1])) {
                printf("\b \b");
                (*length)--;
            }
            while (*length > 0 && !isspace(buffer[*length - 1])) {
                printf("\b \b");
                (*length)--;
            }
        } else if (c == CTRL_D) {
            if (*length == 0) {
                printf("\n");
                break;
            }
        } else if (iscntrl(c) && c != '\n' && c != '\t') {
            printf("%c", CTRL_G);
        } else {
            if (*length < 40) {
                buffer[*length] = c;
                (*length)++;
                printf("%c", c);
            } else {
                printf("\n");
                *length = 0;
            }
        }
    }
}

int main() {
    char buffer[41];
    int length = 0;

    set_terminal_mode(1);

    handle_input(buffer, &length);

    set_terminal_mode(0);

    return 0;
}

