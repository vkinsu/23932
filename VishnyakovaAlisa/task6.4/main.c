#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void timeout_fun(int var) {
    printf("\nTime's out\n");
    exit(0);
}

int main() {
    FILE* ptrFile = fopen("file.txt", "r+b");
    if (ptrFile == NULL) {
        fputs("\nError opening file\n", stderr);
        exit(1);
    }

    size_t size1 = 10;
    char* buffer = (char*)malloc(65536 * sizeof(char));
    if (buffer == NULL) {
        fputs("\nMemory allocation error\n", stderr);
        fclose(ptrFile);
        exit(2);
    }

    int count1 = 0;
    size_t* mas = (size_t*)malloc(sizeof(size_t) * size1);
    if (mas == NULL) {
        fputs("\nMemory allocation error for line numbers\n", stderr);
        free(buffer);
        fclose(ptrFile);
        exit(4);
    }

    mas[count1++] = 0;
    size_t result = fread(buffer, 1, 65536, ptrFile);
    char prev = ' ';
    int ind = 0;

    while (result > 0) {
        for (int i = 0; i < result; i++) {
            char tek = buffer[i];
            if ((prev == '\r' && tek != '\n') || (prev == '\n')) {
                mas[count1++] = ind;
                if (count1 == size1) {
                    size1 += size1 / 2;
                    mas = realloc(mas, sizeof(size_t) * size1);
                    if (mas == NULL) {
                        fputs("\nMemory allocation error for line numbers\n", stderr);
                        free(buffer);
                        fclose(ptrFile);
                        exit(4);
                    }
                }
            }
            ind++;
            prev = tek;
        }
        result = fread(buffer, 1, 65536, ptrFile);
    }

    if (prev == '\r' || prev == '\n') {
        mas[count1++] = ind;
    }

    printf("\n");
    for (int i = 1; i <= count1; i++) {
        fseek(ptrFile, mas[i - 1], SEEK_SET);
        int length = (i == count1) ? ind - mas[i - 1] : mas[i] - mas[i - 1];
        char* lineBuffer = (char*)malloc(length + 1);
        if (lineBuffer == NULL) {
            perror("\nMemory allocation error for line\n");
            free(mas);
            free(buffer);
            fclose(ptrFile);
            exit(5);
        }
        fread(lineBuffer, 1, length, ptrFile);
        lineBuffer[length] = '0'; // Завершаем строку
        printf("\nLine %d:\n%s\n", i, lineBuffer);
        free(lineBuffer);
    }

    while (1) {
        int lineNumber;

        printf("Put number: ");
        signal(SIGALRM, timeout_fun);
        alarm(5);

        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nError reading input.\n");
            alarm(0);
            continue;
        }

        alarm(0);

        if (sscanf(input, "%d", &lineNumber) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            continue;
        }

        if (lineNumber == 0) {
            break;
        }

        if (lineNumber < 1 || lineNumber > count1) {
            printf("\nInvalid line number. Please enter a number from 1 to %d\n", count1);
            continue;
        }
        else {
            fseek(ptrFile, mas[lineNumber - 1], SEEK_SET);
            int length = (lineNumber == count1) ? ind - mas[lineNumber - 1] : mas[lineNumber] - mas[lineNumber - 1];

            char* lineBuffer = (char*)malloc(length + 1); // +1 для '0'
            if (lineBuffer == NULL) {
                perror("\nMemory allocation error for line\n");
                break;
            }
            fread(lineBuffer, 1, length, ptrFile);
            lineBuffer[length] = '0'; // Завершаем строку
            printf("\nLine %d:\n%s\n", lineNumber, lineBuffer);
            free(lineBuffer);
        }
    }

    free(buffer);
    free(mas);
    fclose(ptrFile);
    return 0;
}
