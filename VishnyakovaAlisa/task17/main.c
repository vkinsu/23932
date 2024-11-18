#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

// Настройка терминала для неканонического режима
void setup_terminal(struct termios* saved_settings) {
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, saved_settings); // Сохраняем текущие настройки
    new_settings = *saved_settings;
    new_settings.c_lflag &= ~ICANON; // Отключаем канонический режим
    new_settings.c_lflag &= ~ECHO;    // Отключаем эхо
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings); // Применяем новые настройки
}

// Удаление последнего символа из буфера ввода
void remove_last_char(char* buffer, int* position) {
    if (*position > 0) {
        (*position)--; // Уменьшаем позицию
        buffer[*position] = '\0'; // Удаляем символ
        write(STDOUT_FILENO, "\b \b", 3); // Удаляем символ из вывода
    }
    else {
        write(STDOUT_FILENO, "\a", 1); // Звуковой сигнал, если ничего нет для удаления
    }
}

// Удаление последнего слова из буфера ввода
void remove_last_word(char* buffer, int* position) {
    while (*position > 0 && buffer[*position - 1] == ' ') {
        remove_last_char(buffer, position); // Удаляем пробелы в конце слова
    }
    while (*position > 0 && buffer[*position - 1] != ' ') {
        remove_last_char(buffer, position); // Удаляем символы слова
    }
}

int main() {
    struct termios original_settings;
    setup_terminal(&original_settings); // Настраиваем терминал

    char input_buffer[41] = { 0 }; // Буфер для ввода (максимум 40 символов + нуль-терминатор)
    int current_index = 0; // Текущая позиция в буфере

    while (1) {
        char input_char;
        if (read(STDIN_FILENO, &input_char, 1) <= 0) {
            perror("Ошибка чтения ввода");
            break;
        }

        if (input_char == 4 && current_index == 0) { // Ctrl-D для выхода
            break;
        }
        else if (input_char == 127) { // Символ удаления (Backspace)
            remove_last_char(input_buffer, &current_index);
        }
        else if (input_char == 21) { // Ctrl-U для очистки строки
            while (current_index > 0) {
                remove_last_char(input_buffer, &current_index);
            }
        }
        else if (input_char == 23) { // Ctrl-W для удаления последнего слова
            remove_last_word(input_buffer, &current_index);
        }
        else if (input_char >= 32 && input_char <= 126) { // Допустимые символы
            if (current_index < 40) {
                input_buffer[current_index++] = input_char; // Добавляем символ в буфер
                write(STDOUT_FILENO, &input_char, 1); // Выводим символ на экран
                if (current_index == 40) { // Если достигнут лимит
                    write(STDOUT_FILENO, "\n", 1);
                    current_index = 0; // Сбрасываем индекс
                }
            }
            else {
                write(STDOUT_FILENO, "\7", 1); // Звуковой сигнал при превышении длины
            }
        }
        else {
            write(STDOUT_FILENO, "\a", 1); // Звуковой сигнал для недопустимого ввода
        }
    }

    write(STDOUT_FILENO, "\n", 1); // Печатаем новую строку перед выходом
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings); // Восстанавливаем настройки терминала
    return 0;
}
