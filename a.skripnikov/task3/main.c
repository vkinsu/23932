#include <stdio.h>
#include <unistd.h>

int main(int args, char *argv[])
{
    if (args < 2)
    {
        perror("Incorrect number of arguments!\n");
        return -1;
    }
    // 1
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    // 2
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error of opening file!\n");
    }
    else
    {
        printf("File was opened!\n");
        fclose(file);
    }

    // 3
    setuid(geteuid());

    // 4
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error of opening file!\n");
    }
    else
    {
        printf("File was opened!\n");
        fclose(file);
    }

    return 0;
}