/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double round(double __x);
extern int long lround(double __x);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("round(+2.3) = %+.1f  ", round(2.2999999999999998));
    printf("round(+2.5) = %+.1f  ", round(2.5));
    printf("round(+2.7) = %+.1f\n", round(2.7000000000000002));
    printf("round(-2.3) = %+.1f  ", round(-2.2999999999999998));
    printf("round(-2.5) = %+.1f  ", round(-2.5));
    printf("round(-2.7) = %+.1f\n", round(-2.7000000000000002));
    printf("round(-0.0) = %+.1f\n", round(-0.0));
    printf("round(-Inf) = %+f\n", round(-(__builtin_inff())));
    printf("lround(+2.3) = %ld  ", lround(2.2999999999999998));
    printf("lround(+2.5) = %ld  ", lround(2.5));
    printf("lround(+2.7) = %ld\n", lround(2.7000000000000002));
    printf("lround(-2.3) = %ld  ", lround(-2.2999999999999998));
    printf("lround(-2.5) = %ld  ", lround(-2.5));
    printf("lround(-2.7) = %ld\n", lround(-2.7000000000000002));
    printf("lround(-0.0) = %ld\n", lround(-0.0));
    printf("lround(-Inf) = %ld\n", lround(-(__builtin_inff())));
    feclearexcept(61);
    printf("lround(LONG_MAX+1.5) = %ld\n", lround(9.2233720368547758e+18));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID was raised");
    }
}


