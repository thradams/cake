/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double modf(double __x, double * __iptr);
extern double frexp(double __x, int * __exponent);
extern double logb(double __x);
extern double scalbn(double __x, int __n);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    double f;
    double f3;
    double f2;
    int i;

    f = 123.45;
    printf("Given the number %.2f or %a in hex,\n", f, f);
    f2 = modf(f, &f3);
    printf("modf() makes %.0f + %.2f\n", f3, f2);
    f2 = frexp(f, &i);
    printf("frexp() makes %f * 2^%d\n", f2, i);
    i = logb(f);
    printf("logb()/logb() make %f * %d^%d\n", f / scalbn(1.0, i), 2, i);
    feclearexcept(61);
    printf("logb(0) = %f\n", logb(0));
    if (fetestexcept(4))
    {
        puts("    FE_DIVBYZERO raised");
    }
}


