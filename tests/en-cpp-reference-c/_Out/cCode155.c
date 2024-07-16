//en.cppreference.com/w/c/language/volatile.html
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    clock_t t = clock();
    double d = 0.0;
    for (int n = 0; n < 10000; ++n)
        for (int m = 0; m < 10000; ++m)
            d += d * n * m; // reads from and writes to a non-volatile 
    printf("Modified a non-volatile variable 100m times. "
           "Time used: %.2f seconds\n",
           (double)(clock() - t)/CLOCKS_PER_SEC);
 
    t = clock();
    volatile double vd = 0.0;
    for (int n = 0; n < 10000; ++n)
        for (int m = 0; m < 10000; ++m) {
            double prod = vd * n * m; // reads from a volatile
            vd += prod; // reads from and writes to a volatile
        } 
    printf("Modified a volatile variable 100m times. "
           "Time used: %.2f seconds\n",
           (double)(clock() - t)/CLOCKS_PER_SEC);
}