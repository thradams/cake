/* Cake x86_x64_gcc */

extern double erf(double);
extern double sqrt(double __x);

double phi(double x1, double x2)
{
    return (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2))) / 2;
}


extern int puts(char * __s);
extern int printf(char * __format, ...);

int main(void)
{
    puts("normal variate probabilities:");
    {
        int n;

        n = -4;
        for (; n < 4; ++n)
        printf("[%2d:%2d]: %5.2f%%\n", n, n + 1, 100 * phi(n, n + 1));
    }
    puts("special values:");
    printf("erf(-0) = %f\n", erf(-0.0));
    printf("erf(Inf) = %f\n", erf((__builtin_inff())));
}


