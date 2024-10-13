#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#pragma warning(disable : 4996)

int main() {
    FILE* ptrFile = fopen("file.txt", "r+b");
    if (ptrFile == NULL) {
        fputs("Error opening file", stderr);
        exit(1);
    }

    size_t size1 = 10;
    char* buffer = (char*)malloc(65536* sizeof(char)); 
    if (buffer == NULL) {
        fputs("Memory allocation error", stderr);
        exit(2);
    }
    int count1 = 0;
    size_t* mas = (size_t*)malloc(sizeof(size_t) * size1);
    if (mas == NULL)
    {
        fputs("Memory allocation error for line numbers", stderr);
        free(buffer);
        exit(4);
    }
    mas[count1] = 0;
    count1++;
    int ind = 0;
    size_t result = fread(buffer, 1, 65536, ptrFile);
    char prev = ' ';
    while (result > 0)
    {
       
        for (int i = 0; i < result; i++)
        {
            char tek = buffer[i];
            if ((prev == 0x0d && (tek != 0x0a)) || (prev == 0x0a ))
            {
                mas[count1] = ind;
                count1++;
            }
            if (count1 == size1)
            {
                size1 = size1 + size1 / 2;
                mas = realloc(mas, sizeof(size_t) * size1);
            }
            ind++;
            prev = tek;
        }
       
        result = fread(buffer, 1, 65536, ptrFile);
    }
    if (prev == 0x0d || prev == 0x0a)
    {
        mas[count1++] = ind;
    }

    while (1) {
        int lineNumber;
        printf("Put number: ");
        scanf("%d", &lineNumber);

        if (lineNumber == 0)
        {
            break;
        }

        if (lineNumber < 1 || lineNumber > count1)
        {
            printf("Invalid line number. Please enter a number from 1 to %d\n", count1);
            continue;
        }
        else
        {
            fseek(ptrFile, mas[lineNumber - 1], SEEK_SET);
            int length = 0;
            if (lineNumber == count1)
            {
                 length = ind - mas[lineNumber - 1];
            }
            else
            {
                length = mas[lineNumber] - mas[lineNumber - 1];
            }
            if (length == 0)
            {
                printf("Line %d : \n", lineNumber);
            }
            else
            {
                char* lineBuffer = (char*)malloc(length);
                if (lineBuffer == NULL)
                {
                    perror("Memory allocation error for line");
                    break;
                }
                fread(lineBuffer, 1, length, ptrFile);
                lineBuffer[length - 1] = '\0';
                printf("Line %d: %s\n", lineNumber, lineBuffer);
                free(lineBuffer);
            }
            
           
            
        }
       
    }

    free(buffer);
    free(mas);
    fclose(ptrFile); 
    return 0;
}
