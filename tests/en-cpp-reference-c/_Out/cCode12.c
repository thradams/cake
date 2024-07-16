//en.cppreference.com/w/c/chrono/strftime.html
#include <locale.h>
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    char buff[70];
    struct tm my_time = { .tm_year=112, // = year 2012
                          .tm_mon=9,    // = 10th month
                          .tm_mday=9,   // = 9th day
                          .tm_hour=8,   // = 8 hours
                          .tm_min=10,   // = 10 minutes
                          .tm_sec=20    // = 20 secs
    };
 
    if (strftime(buff, sizeof buff, "%A %c", &my_time))
        puts(buff);
    else
        puts("strftime failed");
 
    setlocale(LC_TIME, "el_GR.utf8");
 
    if (strftime(buff, sizeof buff, "%A %c", &my_time))
        puts(buff);
    else
        puts("strftime failed");
}