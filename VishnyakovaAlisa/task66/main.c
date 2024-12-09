#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#pragma warning(disable : 4996)

void timeout_fun(int var)
{
    printf("\nTimeout\n");
    exit(0);
}

int main() {
    FILE* ptrFile = fopen("file.txt", "r+b");
    if (ptrFile == NULL) {
        fputs("Error opening file", stderr);
        exit(1);
    }
    size_t size1 = 10;
    char* buffer = (char*)malloc(65536 * sizeof(char));
    if (buffer == NULL) {
        fputs("Memory allocation error", stderr);
        exit(2);
    }
    int count1 = 0;
    size_t* mas = (size_t*)malloc(sizeof(size_t) * size1);
    if (mas == NULL) {
        fputs("Memory allocation error for line numbers", stderr);
        free(buffer);
        exit(4);
    }
    mas[count1] = 0;
    count1++;
    int ind = 0;
    size_t result = fread(buffer, 1, 65536, ptrFile);
    char prev = ' ';
    while (result > 0) {
        for (int i = 0; i < result; i++) {
            char tek = buffer[i];
            if ((prev == 0x0d && (tek != 0x0a)) || (prev == 0x0a)) {
                mas[count1] = ind;
                count1++;
            }
            if (count1 == size1) {
                size1 = size1 + size1 / 2;
                mas = realloc(mas, sizeof(size_t) * size1);
            }
            ind++;
            prev = tek;
        }
        result = fread(buffer, 1, 65536, ptrFile);
    }
    if (prev == 0x0d || prev == 0x0a) {
        mas[count1++] = ind;
    }
    printf("\n");
    for (int i = 1; i <= count1; i++)
    {
        fseek(ptrFile, mas[i - 1], SEEK_SET);
        int length = (i == count1) ? ind - mas[i - 1] : mas[i] - mas[i - 1];
        char* lineBuffer = (char*)malloc(length + 1);
        if (lineBuffer == NULL)
        {
            perror("Memory allocation error for line");
            free(mas);
            free(buffer);
            fclose(ptrFile);
            exit(5);
        }
        fread(lineBuffer, 1, length, ptrFile);
        if (length > 0)
        {
            lineBuffer[length - 1] = '\0';
        }
        else
        {
            lineBuffer[0] = '\0';
        }
        printf("Line %d:\n%s\n", i, lineBuffer);
        free(lineBuffer);
    }

    while (1) {
        int lineNumber;

        printf("Put number: ");
        signal(SIGALRM, timeout_fun);
        alarm(5);

        if (scanf("%d", &lineNumber) != 1) {
            printf("Invalid input. Please enter a number.n");
            while (getchar() != 'n'); // очистка ввода
            alarm(0);
            continue;
        }

        alarm(0);

        if (lineNumber == 0) {
            break;
        }

        if (lineNumber < 1 || lineNumber > count1) {
            printf("Invalid line number. Please enter a number from 1 to %dn", count1);
            continue;
        }
        else {
            fseek(ptrFile, mas[lineNumber - 1], SEEK_SET);
            int length = (lineNumber == count1) ? ind - mas[lineNumber - 1] : mas[lineNumber] - mas[lineNumber - 1];

            if (length == 0) {
                printf("Line %d : n", lineNumber);
            }
            else {
                char* lineBuffer = (char*)malloc(length + 1); // +1 для '0'
                if (lineBuffer == NULL) {
                    perror("Memory allocation error for line");
                    break;
                }
                fread(lineBuffer, 1, length, ptrFile);
                lineBuffer[length] = '0'; // корректно завершаем строку
                printf("Line %d: %sn", lineNumber, lineBuffer);
                free(lineBuffer);
            }
        }
    }

    free(buffer);
    free(mas);
    fclose(ptrFile);
    return 0;
}
