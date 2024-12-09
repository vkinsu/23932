#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#define ERASE 127       // Символ удаления
#define KILL 21         // Ctrl-U
#define CTRL_W 23       // Ctrl-W
#define CTRL_D 4        // Ctrl-D
#define CTRL_G 7        // Ctrl-G (звуковой сигнал)
#define MAX_LINE_LEN 40 // Максимальная длина строки

void enable_raw_mode(struct termios *orig_termios)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig_termios);
    raw = *orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios *orig_termios)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

void output_bell()
{
    write(STDOUT_FILENO, "\a", 1); // CTRL-G (звуковой сигнал)
}

void handle_backspace(char *buffer, int *length)
{
    if (*length > 0)
    {
        (*length)--;
        write(STDOUT_FILENO, "\b \b", 3);
    }
}

void handle_kill(char *buffer, int *length)
{
    while (*length > 0)
    {
        write(STDOUT_FILENO, "\b \b", 3);
        (*length)--;
    }
}

void handle_ctrl_w(char *buffer, int *length)
{
    while (*length > 0 && isspace(buffer[*length - 1]))
    {
        write(STDOUT_FILENO, "\b \b", 3);
        (*length)--;
    }
    while (*length > 0 && !isspace(buffer[*length - 1]))
    {
        write(STDOUT_FILENO, "\b \b", 3);
        (*length)--;
    }
}

int main()
{
    struct termios orig_termios;
    enable_raw_mode(&orig_termios);

    char buffer[MAX_LINE_LEN + 1] = {0};
    int length = 0;

    while (1)
    {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);

        if (n <= 0)
            break;

        if (iscntrl(c))
        {
            if (c == CTRL_D) // Завершение программы
            {
                if (length == 0)
                {
                    write(STDOUT_FILENO, "\n", 1);
                    break;
                }
            }
            else if (c == ERASE) // Удаление символа
            {
                handle_backspace(buffer, &length);
            }
            else if (c == KILL) // Удаление строки
            {
                handle_kill(buffer, &length);
            }
            else if (c == CTRL_W) // Удаление слова
            {
                handle_ctrl_w(buffer, &length);
            }
            else
            {
                output_bell();
            }
        }
        else
        {
            if (length < MAX_LINE_LEN)
            {
                buffer[length++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
            else
            {
                write(STDOUT_FILENO, "\n", 1);
                buffer[length] = '\0';
                write(STDOUT_FILENO, buffer, length);
                write(STDOUT_FILENO, "\n", 1);
                length = 0;
                buffer[length++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    disable_raw_mode(&orig_termios);
    return 0;
}
