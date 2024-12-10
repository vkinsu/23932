#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>

static int table[65536];
int file;
char *src;
off_t sof, current;

char read_from_map()
{
    if (current == sof) return -1;

    return src[current++];
}

void mseek(off_t glob_pos)
{
    current = glob_pos;
}

void end_of_time()
{
    off_t pos = lseek(file, 0, SEEK_SET);
    char c;
    printf("\nThe time is over\n");
    while (read(file, &c, 1) != 0)
    {
        printf("%c", c);
    }
    printf("\n");
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGALRM, end_of_time);
    file = open("file.txt", O_RDONLY);
    sof = lseek(file, 0, SEEK_END);
    src = (char*)mmap(0, sof, PROT_READ, MAP_PRIVATE, file, 0);
    
    char c;
    off_t index = 0;
    off_t line = 1;
    table[0] = -1;
    while ((c = read_from_map()) != -1)
    {
        if (c == '\n')
        {
            table[line++] = index;
        }
        index++;
    }

    table[line++] = sof;

    printf("File has read\n");
    off_t find_line;
    alarm(5);
    while (1)
    {
        printf("Enter number of line: ");
        scanf("%ld", &find_line);
        alarm(0);
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

        mseek(table[find_line-1] + 1);

        for (int i = 0; i < table[find_line] - table[find_line-1] - 1; i++)
        {
            c = read_from_map();
            printf("%c ", c);
        }   
        printf("\n");     
    }
    exit(EXIT_SUCCESS);
}
