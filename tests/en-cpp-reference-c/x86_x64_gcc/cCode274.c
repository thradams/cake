/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double sinh(double __x);
extern double log(double __x);
extern double cosh(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("sinh(1) = %f\nsinh(-1)=%f\n", sinh(1), sinh(-1));
    printf("log(sinh(1) + cosh(1))=%f\n", log(sinh(1) + cosh(1)));
    printf("sinh(+0) = %f\nsinh(-0)=%f\n", sinh(0.0), sinh(-0.0));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("sinh(710.5) = %f\n", sinh(710.5));
    if ((*__errno_location()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_OVERFLOW raised");
    }
}


