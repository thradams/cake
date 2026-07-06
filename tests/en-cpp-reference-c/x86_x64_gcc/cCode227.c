/* Cake x86_x64_gcc */

extern double erfc(double);
extern double sqrt(double __x);

double normalCDF(double x)
{
    return erfc(-x / sqrt(2)) / 2;
}


extern int puts(char * __s);
extern int printf(char * __format, ...);

int main(void)
{
    puts("normal cumulative distribution function:");
    {
        double n;

        n = 0;
        for (; n < 1; n += 0.10000000000000001)
        printf("normalCDF(%.2f) %5.2f%%\n", n, 100 * normalCDF(n));
    }
    printf("special values:\nerfc(-Inf) = %f\nerfc(Inf) = %f\n", erfc(-(__builtin_inff())), erfc((__builtin_inff())));
}


