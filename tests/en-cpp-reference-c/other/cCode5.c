//en.cppreference.com/w/c/chrono/clock.html
#ifndef __STDC_NO_THREADS__
    #include <threads.h>
#else
    // POSIX alternative
    #define _POSIX_C_SOURCE 199309L
    #include <pthread.h>
#endif
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
// the function f() does some time-consuming work
int f(void* thr_data) // return void* in POSIX
{
    (void) thr_data;
    volatile double d = 0;
    for (int n = 0; n < 10000; ++n)
       for (int m = 0; m < 10000; ++m)
           d += d * n * m;
    return 0;
}
 
int main(void)
{
    struct timespec ts1, tw1; // both C11 and POSIX
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1); // POSIX
    clock_gettime(CLOCK_MONOTONIC, &tw1); // POSIX; use timespec_get in C11
    clock_t t1 = clock();
 
#ifndef __STDC_NO_THREADS__
    thrd_t thr1, thr2;  // C11; use pthread_t in POSIX
    thrd_create(&thr1, f, NULL); // C11; use pthread_create in POSIX
    thrd_create(&thr2, f, NULL);
    thrd_join(thr1, NULL); // C11; use pthread_join in POSIX
    thrd_join(thr2, NULL);
#endif
 
    struct timespec ts2, tw2;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
    clock_gettime(CLOCK_MONOTONIC, &tw2);
    clock_t t2 = clock();
 
    double dur = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
    double posix_dur = 1000.0 * ts2.tv_sec + 1e-6 * ts2.tv_nsec
                       - (1000.0 * ts1.tv_sec + 1e-6 * ts1.tv_nsec);
    double posix_wall = 1000.0 * tw2.tv_sec + 1e-6 * tw2.tv_nsec
                        - (1000.0 * tw1.tv_sec + 1e-6 * tw1.tv_nsec);
 
    printf("CPU time used (per clock()): %.2f ms\n", dur);
    printf("CPU time used (per clock_gettime()): %.2f ms\n", posix_dur);
    printf("Wall time passed: %.2f ms\n", posix_wall);
}