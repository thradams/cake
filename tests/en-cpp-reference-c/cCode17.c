//en.cppreference.com/w/c/chrono/timespec_getres.html
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    char buff[128];
    struct timespec ts;
    const int res = timespec_getres(&ts, TIME_UTC);
    if (res == TIME_UTC) {
        struct tm timer;
        strftime(buff, sizeof buff, "%D %T", gmtime_r(&ts.tv_sec, &timer));
        printf("Time resolution info: %s.%09ld UTC\n", buff, ts.tv_nsec);
    } else {
        printf("TIME_UTC base is not supported.");
    }
}