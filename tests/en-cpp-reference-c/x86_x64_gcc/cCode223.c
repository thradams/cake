/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double cosh(double __x);
extern double log(double __x);
extern double sinh(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("cosh(1) = %f\ncosh(-1)= %f\n", cosh(1), cosh(-1));
    printf("log(sinh(1) + cosh(1))=%f\n", log(sinh(1) + cosh(1)));
    printf("cosh(+0) = %f\ncosh(-0) = %f\n", cosh(0.0), cosh(-0.0));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("cosh(710.5) = %f\n", cosh(710.5));
    if ((*__errno_location()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_OVERFLOW raised");
    }
}


