#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_LINE_LENGTH 40
#define ERASE 127
#define KILL 21
#define CTRL_W 23
#define CTRL_D 4
#define CTRL_G 7

struct termios orig_termios;

void reset_terminal_mode() {
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void set_terminal_mode() {
	struct termios new_termios;
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(reset_terminal_mode);

	new_termios = orig_termios;
	new_termios.c_lflag &= ~(ICANON | ECHO);
	new_termios.c_cc[VMIN] = 1;
	new_termios.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void erase_last_word(char *line, int *length) {
	if (*length == 0) return;
	while (*length > 0 && line[*length - 1] == ' ') (*length)--;
	while (*length > 0 && line[*length - 1] != ' ') (*length)--;
	line[*length] = '\0';
}

void process_input() {
	char line[MAX_LINE_LENGTH + 1] = {0};
	int length = 0;
	char c;

	while (1) {
		read(STDIN_FILENO, &c, 1);

		if (c == CTRL_D && length == 0) {
			break;
		} else if (c == ERASE) {
			if (length > 0) {
				length--;
				printf("\b \b");
			}
		} else if (c == KILL) {
			while (length > 0) {
				printf("\b \b");
				length--;
			}
			line[0] = '\0';
		} else if (c == CTRL_W) {
			erase_last_word(line, &length);
			printf("\r\033[K");
			printf("%s", line);
			fflush(stdout);
		} else if (c >= 32 && c <= 126) {
			if (length < MAX_LINE_LENGTH) {
				line[length++] = c;
				line[length] = '\0';
				putchar(c);
			} else {
				putchar('\n');
				line[0] = c;
				length = 1;
				line[length] = '\0';
				putchar(c);
			}
		} else {
			putchar(CTRL_G);
		}

		fflush(stdout);
	}
}

int main() {
	set_terminal_mode();
	printf("Start typing (press CTRL-D at the beginning of the line to exit)\n");
	process_input();
	printf("\nExiting program\n");
	return 0;
}

