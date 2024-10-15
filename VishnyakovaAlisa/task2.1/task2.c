#include <stdlib.h>
#pragma warning(disable : 4996)
extern char* tzname[];
int main()
{

    time_t now;
    struct tm* sp;
    setenv("TZ", "PST8PDT", 1);
    tzset();
    (void)time(&now);
    printf("%s", ctime(&now));
    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year - 100, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    exit(0);
}