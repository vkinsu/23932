#include <stdio.h>
#include <threads.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#pragma warning(disable : 4996)

#define READ_BLOCK_SIZE 512

volatile int v_row = -1;
volatile int v_read = 0;

int input_thread(void* value) {
    int r;
    v_read = 1;
    scanf("%d", &r);
    v_row = r;
    v_read = 0;
    return 0;
}

int get_row()
{
    return v_row;
}

int get_read()
{
    return v_read;
}

int main()
{
    FILE* ptrFile = fopen("file.txt", "r+b");
    if (ptrFile == NULL)
    {
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

    mas[count1++] = 0;
    int ind = 0;
    size_t result = fread(buffer, 1, 65536, ptrFile);
    char prev = ' ';

    while (result > 0) {
        for (int i = 0; i < result; i++) {
            char tek = buffer[i];
            if ((prev == 0x0d && tek != 0x0a) || (prev == 0x0a)) {
                mas[count1++] = ind;
                if (count1 == size1) {
                    size1 += size1 / 2;
                    mas = realloc(mas, sizeof(size_t) * size1);
                }
            }
            ind++;
            prev = tek;
        }
        result = fread(buffer, 1, 65536, ptrFile);
    }

    if (prev == 0x0d || prev == 0x0a) {
        mas[count1++] = ind;
    }
    while (1)
    {
        printf("Input row number (5 second wait): ");

        thrd_t thread;
        thrd_create(&thread, input_thread, NULL);

        int msec, isRead;
        v_row = -1;
        v_read = 1;
        clock_t start_t = clock();
        struct timespec sleep = { 0, 100000000 };

        do {
            thrd_sleep(&sleep, NULL);
            clock_t diff_t = clock() - start_t;
            msec = diff_t * 1000 / CLOCKS_PER_SEC;
            isRead = get_read();
        } while ((msec < 5000) && (isRead == 1));


        if (isRead == 1)
        {
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
            break;
        }
        else
        {
            thrd_join(thread, NULL);
            int r = get_row();
            if (r == 0)
            {
                break;
            }
            if (r < 1 || r > count1)
            {
                printf("Invalid line number. Please enter a number from 1 to %d\n", count1);
            }
            else
            {
                fseek(ptrFile, mas[r - 1], SEEK_SET);
                int length = (r == count1) ? ind - mas[r - 1] : mas[r] - mas[r - 1];
                char* lineBuffer = (char*)malloc(length + 1);
                if (lineBuffer == NULL) {
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
                printf("Line %d:\n%s\n", r, lineBuffer);
                free(lineBuffer);
            }
        }
    }


    free(buffer);
    free(mas);
    fclose(ptrFile);

    return 0;
}
