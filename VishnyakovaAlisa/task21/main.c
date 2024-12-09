#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int k = 0; // Счетчик сигналов

// Обработчик для SIGINT
void mysignal(int var) {
    k += 1; // Увеличиваем счетчик
    printf("\7\n"); // Звуковой сигнал
    signal(SIGINT, mysignal);
}

// Обработчик для SIGQUIT
void mysignal2(int var) {
    printf("\n%d\n", k); // Выводим количество полученных SIGINT
    exit(0); // Завершаем программу
}

int main() {
    // Устанавливаем обработчики сигналов
    signal(SIGINT, mysignal);
    signal(SIGQUIT, mysignal2);

    // Бесконечный цикл, чтобы программа не завершалась
    while (1)
    {
        pause(); // Ожидание сигнала
    }

    exit(0); // Завершение программы (не достигнет здесь)
}