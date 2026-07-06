/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double pow(double __x, double __y);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("pow(2, 10) = %f\n", pow(2, 10));
    printf("pow(2, 0.5) = %f\n", pow(2, 0.5));
    printf("pow(-2, -3) = %f\n", pow(-2, -3));
    printf("pow(-1, NAN) = %f\n", pow(-1, (__builtin_nanf(""))));
    printf("pow(+1, NAN) = %f\n", pow(1, (__builtin_nanf(""))));
    printf("pow(INFINITY, 2) = %f\n", pow((__builtin_inff()), 2));
    printf("pow(INFINITY, -1) = %f\n", pow((__builtin_inff()), -1));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("pow(-1, 1/3) = %f\n", pow(-1, 0.33333333333333331));
    if ((*__errno_location()) == 33)
    {
        perror("    errno == EDOM");
    }
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
    feclearexcept(61);
    printf("pow(-0, -3) = %f\n", pow(-0.0, -3));
    if (fetestexcept(4))
    {
        puts("    FE_DIVBYZERO raised");
    }
}


