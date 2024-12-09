#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    setenv("TZ", "America/Los_Angeles", 1);
    time_t now;
    (void) time( &now );
    printf("%s\n", ctime( &now ) );
    exit(0);
}