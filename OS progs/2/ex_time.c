 #include <sys/types.h>
 #include <stdio.h>
 #include <time.h>
 #include <stdlib.h>
 //extern char *tzname[];
 
 
int main() {
    time_t now;
    struct tm *sp;
    (void) time( &now );
    printf("%s", ctime( &now ) );
    sp = gmtime(&now);
    sp->tm_hour -= 7;
    if (sp->tm_hour < 0) sp->tm_hour += 24;
    printf("%d/%d/%02d %d:%02d\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min/*, tzname[sp->tm_isdst]*/);
    return 0;
}
