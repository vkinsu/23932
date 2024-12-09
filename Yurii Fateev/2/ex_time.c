 #include <sys/types.h>
 #include <stdio.h>
 #include <time.h>
 #include <stdlib.h>
 //extern char *tzname[];
 
 
int main() {
    time_t now;
    struct tm *sp;
    (void) time( &now );
    printf("In our time area:\n");
    printf("%s", ctime( &now ) );
    sp = gmtime(&now);
    sp->tm_hour -= 7;
    if (sp->tm_hour < 0) {
        sp->tm_hour += 24;
        sp->tm_mday -= 1;
    }
    printf("In California time area:\n");
    printf("%d/%d/%d %d:%02d\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year - 100, sp->tm_hour,
        sp->tm_min/*, tzname[sp->tm_isdst]*/);
    return 0;
}
