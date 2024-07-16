//en.cppreference.com/w/c/thread/thrd_yield.html
#include <stdio.h>
#include <time.h>
#include <threads.h>
 
// utility function: difference between timespecs in microseconds
double usdiff(struct timespec s, struct timespec e)
{
    double sdiff = difftime(e.tv_sec, s.tv_sec);
    long nsdiff = e.tv_nsec - s.tv_nsec;
    if(nsdiff < 0) return 1000000*(sdiff-1) + (1000000000L+nsdiff)/1000.0;
    else return 1000000*(sdiff) + nsdiff/1000.0;
}
 
// busy wait while yielding
void sleep_100us()
{
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);
    do {
        thrd_yield();
        timespec_get(&end, TIME_UTC);
    } while(usdiff(start, end) < 100.0);
}
 
int main()
{
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);
    sleep_100us();
    timespec_get(&end, TIME_UTC);
    printf("Waited for %.3f us\n", usdiff(start, end));
}