//en.cppreference.com/w/c/chrono/timespec_get.html
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
}