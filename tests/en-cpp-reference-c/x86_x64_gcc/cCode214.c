/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double asin(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("asin( 1.0) = %+f, 2*asin( 1.0)=%+f\n", asin(1), 2 * asin(1));
    printf("asin(-0.5) = %+f, 6*asin(-0.5)=%+f\n", asin(-0.5), 6 * asin(-0.5));
    printf("asin(0.0) = %1f, asin(-0.0)=%f\n", asin(0.0), asin(-0.0));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("asin(1.1) = %f\n", asin(1.1000000000000001));
    if ((*__errno_location()) == 33)
    {
        perror("    errno == EDOM");
    }
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


