#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() 
{
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    printf("Real UID: %d\n", real_uid);
    printf("Effective UID: %d\n", effective_uid);
    FILE* file = fopen("file", "r");
    if (file == NULL)
    {
        perror("Open error");
    }
    else if (fclose(file) == EOF)
    {
        perror("Close error");
    }
    if (setuid(real_uid) == -1)
    {
        perror("Error UID");
       
    }
    real_uid = getuid();
    effective_uid = geteuid();
    printf("After UID:\n");
    printf("Real UID: %d\n", real_uid);
    printf("Effective UID: %d\n", effective_uid);

    file = fopen("file", "r");
    if (file == NULL)
    {
        perror("Open error");
    }
    else if (fclose(file) == EOF)
    {
        perror("Close error");
    }

    return 0;
}
