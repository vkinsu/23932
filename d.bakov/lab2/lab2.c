#include <stdio.h>
#include <time.h>
#include <stdlib.h>


extern char *tzname[];

int main() {
 
    time_t current_time;

   
    struct tm *time_info;

    
    putenv("TZ=America/Los_Angeles");


    current_time = time(NULL);

    
    printf("Current time: %s", ctime(&current_time));

    time_info = localtime(&current_time);

    
    printf("Formatted time: %d/%d/%02d %d:%02d %s\n",
           time_info->tm_mon + 1, 
           time_info->tm_mday,    
           time_info->tm_year - 100, 
           time_info->tm_hour,    
           time_info->tm_min,     
           tzname[time_info->tm_isdst]); 
    
    return 0;
}
