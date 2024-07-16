//en.cppreference.com/w/c/chrono/timespec.html
#include <stdio.h>
#include <time.h>
#include <stdint.h>
 
int main(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
    printf("Raw timespec.time_t: %jd\n", (intmax_t)ts.tv_sec);
    printf("Raw timespec.tv_nsec: %09ld\n", ts.tv_nsec);
}