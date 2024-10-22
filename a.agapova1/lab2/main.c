#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
        setenv("TZ", "America/Los_Angeles", 1);
        tzset();
        time_t timeNow = time(NULL);
        struct tm *local_time = localtime(&timeNow);
        printf("%d/%d/%02d %d:%02d\n",
                local_time -> tm_mon + 1, local_time -> tm_mday,
                local_time -> tm_year + 1900, local_time -> tm_hour,
                local_time -> tm_min);
	exit(0);
}
