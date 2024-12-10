#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

static int table[65536];

int main()
{
    int file = open("file.txt", O_RDONLY);

    off_t pos = lseek(file, 0, SEEK_SET);

    char c;
    off_t index = 0;
    off_t line = 1;
    table[0] = -1;
    while (read(file, &c, 1) != 0)
    {
        if (c == '\n')
        {
            table[line++] = index;
        }
        index++;
    }
    pos = lseek(file, 0, SEEK_END);
    table[line++] = pos;

    printf("File read\n");
    off_t find_line;
    while (1)
    {
        printf("Enter number of line: ");
        scanf("%ld", &find_line);
        if (find_line < 0 || find_line > line-2) 
        {
            printf("Wrong number of line!\n");
            continue;
        }

        if (find_line == 0)
        {
            printf("Program is shuted down\n");
            exit(EXIT_SUCCESS);
        }

        pos = lseek(file, table[find_line-1] + 1, SEEK_SET);

        for (int i = 0; i < table[find_line] - table[find_line-1] - 1; i++)
        {
            read(file, &c, 1);
            printf("%c ", c);
        }   
        printf("\n");     
    }
    exit(EXIT_SUCCESS);
}
