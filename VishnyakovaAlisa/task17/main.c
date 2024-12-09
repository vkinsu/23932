#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define KILL_STRING "\r033[K"
#define ERASE_STRING "\b \b"
#define BEEP_SOUND "x07"

int main()
{
    char input_buffer[41];
    int input_index = 0;
    struct termios original_termios, new_termios;

    // Получаем текущие настройки терминала
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;

    // Настраиваем терминал для неканонического ввода без эха
    new_termios.c_lflag &= ~(ECHO | ICANON);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;

    // Устанавливаем новые настройки терминала
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
    new_termios.c_cc[VERASE] = 8;   // Backspace
    new_termios.c_cc[VWERASE] = 23; // Ctrl+W
    new_termios.c_cc[VKILL] = 21;   // Ctrl+U
    new_termios.c_cc[VEOF] = 4;     // Ctrl+D

    while (1)
    {
        char character;
        read(STDIN_FILENO, &character, 1);

        if (character == 10) // Enter
        {
            input_index = 0;
            write(1, &character, 1);
            break;
        }

        if (character == 4 && input_index == 0) // Ctrl+D
            break;

        if (character == 127) // Backspace
        {
            if (input_index > 0)
            {
                input_index--;
                write(1, ERASE_STRING, strlen(ERASE_STRING));
                continue;
            }
        }
        else if (character == 21) // Ctrl+U
        {
            write(1, KILL_STRING, strlen(KILL_STRING));
            input_index = 0;
            continue;
        }
        else if (character == 23) // Ctrl+W
        {
            while (input_index > 0 && input_buffer[input_index - 1] == ' ')
            {
                input_index--;
                write(1, ERASE_STRING, strlen(ERASE_STRING));
            }
            while (input_index > 0 && input_buffer[input_index - 1] != ' ')
            {
                input_index--;
                write(1, ERASE_STRING, strlen(ERASE_STRING));
            }
            continue;
        }
        else if (input_index < 40 && (character >= 32 && character <= 126))
        {
            input_buffer[input_index++] = character;
            write(1, &character, 1);
        }
        else if (character >= 32 && character <= 126)
        {
            input_index = 0;
            write(1, "\n", 1);
            input_buffer[input_index++] = character;
            write(1, &character, 1);
        }
        else
        {
            write(1, BEEP_SOUND, strlen(BEEP_SOUND));
            continue;
        }
    }

    // Восстанавливаем оригинальные настройки терминала
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    return 0;
}

