/* Cake x86_x64_gcc */

extern int fesetround(int __rounding_direction);
extern int printf(char * __format, ...);
extern double rint(double __x);
extern int long lrint(double __x);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    fesetround(0);
    printf("rounding to nearest (halfway cases to even):\nrint(+2.3) = %+.1f  ", rint(2.2999999999999998));
    printf("rint(+2.5) = %+.1f  ", rint(2.5));
    printf("rint(+3.5) = %+.1f\n", rint(3.5));
    printf("rint(-2.3) = %+.1f  ", rint(-2.2999999999999998));
    printf("rint(-2.5) = %+.1f  ", rint(-2.5));
    printf("rint(-3.5) = %+.1f\n", rint(-3.5));
    fesetround(1024);
    printf("rounding down: \nrint(+2.3) = %+.1f  ", rint(2.2999999999999998));
    printf("rint(+2.5) = %+.1f  ", rint(2.5));
    printf("rint(+3.5) = %+.1f\n", rint(3.5));
    printf("rint(-2.3) = %+.1f  ", rint(-2.2999999999999998));
    printf("rint(-2.5) = %+.1f  ", rint(-2.5));
    printf("rint(-3.5) = %+.1f\n", rint(-3.5));
    printf("rounding down with lrint: \nlrint(+2.3) = %ld  ", lrint(2.2999999999999998));
    printf("lrint(+2.5) = %ld  ", lrint(2.5));
    printf("lrint(+3.5) = %ld\n", lrint(3.5));
    printf("lrint(-2.3) = %ld  ", lrint(-2.2999999999999998));
    printf("lrint(-2.5) = %ld  ", lrint(-2.5));
    printf("lrint(-3.5) = %ld\n", lrint(-3.5));
    printf("lrint(-0.0) = %ld\n", lrint(-0.0));
    printf("lrint(-Inf) = %ld\n", lrint(-(__builtin_inff())));
    feclearexcept(61);
    printf("rint(1.1) = %.1f\n", rint(1.1000000000000001));
    if (fetestexcept(32))
    {
        puts("    FE_INEXACT was raised");
    }
    feclearexcept(61);
    printf("lrint(LONG_MIN-2048.0) = %ld\n", lrint(-9.2233720368547779e+18));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID was raised");
    }
}


