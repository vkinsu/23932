#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_INPUT_LENGTH 40
#define CTRL_D 4
#define CTRL_W 23
#define BACKSPACE 127
#define CTRL_U 21
#define CTRL_G 7

void set_raw_mode(struct termios* original_settings) {
    struct termios raw_settings;

    if (tcgetattr(STDIN_FILENO, original_settings) == -1) {
        perror("Failed to get terminal settings");
        exit(1);
    }

    raw_settings = *original_settings;
    raw_settings.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw_settings) == -1) {
        perror("Failed to set terminal to raw mode");
        exit(1);
    }
}

void reset_terminal_mode(struct termios* original_settings) {
    if (tcsetattr(STDIN_FILENO, TCSANOW, original_settings) == -1) {
        perror("Failed to restore terminal mode");
    }
}

void process_input() {
    char input_buffer[MAX_INPUT_LENGTH + 1] = { 0 };
    int cursor_position = 0;
    char ch;

    while (1) {
        if (read(STDIN_FILENO, &ch, 1) <= 0) {
            perror("Error reading input");
            break;
        }

        if (ch == CTRL_D && cursor_position == 0) {
            break;
        }
        else if (ch == BACKSPACE) {
            if (cursor_position > 0) {
                cursor_position--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else if (ch == CTRL_U) {
            while (cursor_position > 0) {
                cursor_position--;
                printf("\b \b");
            }
            fflush(stdout);
        }
        else if (ch == CTRL_W) {
            while (cursor_position > 0 && input_buffer[cursor_position - 1] == ' ') {
                cursor_position--;
                printf("\b \b");
            }
            while (cursor_position > 0 && input_buffer[cursor_position - 1] != ' ') {
                cursor_position--;
                printf("\b \b");
            }
            fflush(stdout);
        }
        else if (ch == '033') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) break;
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) break;

            printf("^G");
            putchar(CTRL_G);
            fflush(stdout);
            cursor_position += 2;
        }
        else if (ch >= 1 && ch < 32 && ch != CTRL_D && ch != CTRL_W && ch != BACKSPACE && ch != CTRL_U) {
            putchar(CTRL_G);
            fflush(stdout);
            cursor_position += 2;
        }
        else {
            if (cursor_position < MAX_INPUT_LENGTH) {
                input_buffer[cursor_position++] = ch;
                putchar(ch);
                fflush(stdout);
            }
            else if (ch == ' ') {
                putchar('\n');
                cursor_position = 0;
                fflush(stdout);
            }
        }
    }
}

int main() {
    struct termios original_settings;

    set_raw_mode(&original_settings);

    printf("Enter text (CTRL-D to exit):\n");
    fflush(stdout);

    process_input();

    reset_terminal_mode(&original_settings);

    printf("\nProgram terminated.\n");
    return 0;
}
