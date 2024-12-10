#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


#include <stdlib.h>
#include <stdio.h>


#define LEN 40

#define DEL  "\33[D\33[K"
#define DEL_ALL "\33[2K\r"

struct termios terminal, w_o_echo_and_canon;

void set() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal);
    return;
}

int main() {
    tcgetattr(STDIN_FILENO, &terminal);
    atexit(set);
    w_o_echo_and_canon = terminal;
    w_o_echo_and_canon.c_lflag &= ~(ECHO | ICANON);
    w_o_echo_and_canon.c_cc[VMIN] = 1;
    w_o_echo_and_canon.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &w_o_echo_and_canon);
    char current_line[LEN + 1];
    current_line[0] = '\0';
    char current_symbol;
    int current_lenght;
    while (read(STDIN_FILENO, &current_symbol, 1)) {
        current_lenght = strlen(current_line);
        if (!isprint(current_symbol)) {
            if (current_symbol == w_o_echo_and_canon.c_cc[VERASE]) {
                if (current_lenght != 0) {
                    current_line[current_lenght - 1] = '\0';
                    write(1, DEL, strlen(DEL));
                }

            }
            else if (current_symbol == w_o_echo_and_canon.c_cc[VWERASE]) {
                int i = current_lenght - 1;
                char word_started = 0;
                while (i >= 0) {
                    if (current_line[i] == ' ' && word_started == 0) {
                        write(1, DEL, strlen(DEL));
                        current_line[i] = '\0';
                    }
                    else if (current_line[i] != ' ' && word_started == 0) {
                        word_started = 1;
                        write(1, DEL, strlen(DEL));
                        current_line[i] = '\0';
                    }
                    else if (current_line[i] != ' ' && word_started == 1) {
                        write(1, DEL, strlen(DEL));
                        current_line[i] = '\0';
                    }
                    else if (current_line[i] == ' ' && word_started == 1) {
                        break;
                    }
                    i--;
                }

            }
            else if (current_symbol == w_o_echo_and_canon.c_cc[VKILL]) {
                write(1, DEL_ALL, strlen(DEL_ALL));
                current_line[0] = '\0';

            }
            else if (current_symbol == '\004') {
                if (current_lenght == 0) {
                    return(0);
                }
            }
            else {
                write(1, "\x07", 1);
            }

        }
        else {
            if (current_lenght == LEN) {
                write(1, "\n", 1);
                current_lenght = 0;
            }
            current_line[current_lenght] = current_symbol;
            current_line[current_lenght + 1] = '\0';
            write(1, &current_symbol, 1);
        }
    }
    return 0;
}
