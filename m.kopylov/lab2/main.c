#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



int main()
{
    uid_t ruid = getuid(), euid = geteuid();
    printf("ruid:\t%u\neuid:\t%u\n", ruid, euid);

    FILE* fin = fopen("ssf.txt", "r");
    if (fin == NULL)
    {
        perror("Error with opening file\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fclose(fin);
    }

    printf("Changing ruid to %u\n", euid);
    setuid(euid);

    fin = fopen("ssf.txt", "r");
    if (fin == NULL)
    {
        perror("Error with opening\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fclose(fin);
    }

    exit(EXIT_SUCCESS);
}
