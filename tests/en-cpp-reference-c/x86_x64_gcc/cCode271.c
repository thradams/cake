/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double scalbn(double __x, int __n);
extern double nextafter(double __x, double __y);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("scalbn(7, -4) = %f\n", scalbn(7, -4));
    printf("scalbn(1, -1074) = %g (minimum positive subnormal double)\n", scalbn(1, -1074));
    printf("scalbn(nextafter(1,0), 1024) = %g (largest finite double)\n", scalbn(nextafter(1, 0), 1024));
    printf("scalbn(-0, 10) = %f\n", scalbn(-0.0, 10));
    printf("scalbn(-Inf, -1) = %f\n", scalbn(-(__builtin_inff()), -1));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("scalbn(1, 1024) = %f\n", scalbn(1, 1024));
    if ((*__errno_location()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_OVERFLOW raised");
    }
}


