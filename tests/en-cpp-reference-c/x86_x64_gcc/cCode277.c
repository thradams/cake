/* Cake x86_x64_gcc */

extern double acos(double __x);
extern int printf(char * __format, ...);
extern double tan(double __x);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    double pi;

    pi = acos(-1);
    printf("tan(pi*1/4) = %+f\n", tan(pi * 1 / 4));
    printf("tan(pi*3/4) = %+f\n", tan(pi * 3 / 4));
    printf("tan(pi*5/4) = %+f\n", tan(pi * 5 / 4));
    printf("tan(pi*7/4) = %+f\n", tan(pi * 7 / 4));
    printf("tan(+0) = %f\n", tan(0.0));
    printf("tan(-0) = %f\n", tan(-0.0));
    feclearexcept(61);
    printf("tan(INFINITY) = %f\n", tan((__builtin_inff())));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


