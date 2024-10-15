#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];

main()
{
    time_t now;
    struct tm *sp;
    setenv("TZ", "America/Los_Angeles", 1);
    (void) time( &now );
    printf("%s", ctime( &now ));
    exit(0);
}
